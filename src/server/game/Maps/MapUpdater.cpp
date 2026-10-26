/*
 * This file is part of the AzerothCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "MapUpdater.h"
#include "DatabaseEnv.h"
#include "LFGMgr.h"
#include "Log.h"
#include "Map.h"
#include "MapMgr.h"
#include "Metric.h"
#include "Timer.h"
#include <algorithm>
#include <cstring>
#include <initializer_list>

namespace
{
    struct Breadcrumb
    {
        std::atomic<uint32> mapId{0};
        std::atomic<uint32> instanceId{0};
        std::atomic<uint32> startMs{0};
        std::atomic<uint64> botGuid{0};
        char stage[96] = {};
    };

    constexpr std::size_t MAX_TRACE_SLOTS = 64;
    Breadcrumb s_traceSlots[MAX_TRACE_SLOTS];
    std::atomic<std::size_t> s_traceSlotCount{0};
    thread_local Breadcrumb* t_traceSlot = nullptr;

    Breadcrumb* TraceSlot()
    {
        if (!t_traceSlot)
        {
            std::size_t index = s_traceSlotCount.fetch_add(1);
            if (index < MAX_TRACE_SLOTS)
                t_traceSlot = &s_traceSlots[index];
        }
        return t_traceSlot;
    }
}

void MapWorkerTrace::BeginMap(uint32 mapId, uint32 instanceId)
{
    if (Breadcrumb* slot = TraceSlot())
    {
        slot->botGuid = 0;
        slot->stage[0] = 0;
        slot->instanceId = instanceId;
        slot->startMs = getMSTime();
        slot->mapId = mapId + 1; // 0 = idle
    }
}

void MapWorkerTrace::EndMap()
{
    if (Breadcrumb* slot = TraceSlot())
        slot->mapId = 0;
}

void MapWorkerTrace::SetBot(uint64 guid)
{
    if (Breadcrumb* slot = TraceSlot())
    {
        slot->botGuid = guid;
        slot->stage[0] = 0;
    }
}

void MapWorkerTrace::SetStage(char const* prefix, char const* name)
{
    if (Breadcrumb* slot = TraceSlot())
    {
        std::size_t used = 0;
        for (char const* part : { prefix, name })
        {
            if (!part)
                continue;
            std::size_t len = std::min(std::strlen(part), sizeof(slot->stage) - 1 - used);
            std::memcpy(slot->stage + used, part, len);
            used += len;
        }
        slot->stage[used] = 0;
    }
}

std::string MapWorkerTrace::Dump(uint32 nowMs)
{
    std::string out;
    std::size_t count = std::min(s_traceSlotCount.load(), MAX_TRACE_SLOTS);
    for (std::size_t i = 0; i < count; ++i)
    {
        Breadcrumb& slot = s_traceSlots[i];
        char stage[sizeof(slot.stage)];
        std::memcpy(stage, slot.stage, sizeof(stage));
        stage[sizeof(stage) - 1] = 0;
        uint32 map = slot.mapId.load();
        out += "slot " + std::to_string(i) + ": ";
        if (map)
            out += "map " + std::to_string(map - 1) + " instance " + std::to_string(slot.instanceId.load()) +
                   " for " + std::to_string(getMSTimeDiff(slot.startMs.load(), nowMs)) + " ms";
        else
            out += "idle";
        out += " | bot guid " + std::to_string(slot.botGuid.load()) + " | stage " + stage + '\n';
    }
    return out;
}

class UpdateRequest
{
public:
    UpdateRequest() = default;
    virtual ~UpdateRequest() = default;

    virtual void call() = 0;
};

class MapUpdateRequest : public UpdateRequest
{
public:
    MapUpdateRequest(Map& m, MapUpdater& u, uint32 d, uint32 sd)
        : m_map(m), m_updater(u), m_diff(d), s_diff(sd)
    {
    }

    void call() override
    {
        METRIC_TIMER("map_update_time_diff", METRIC_TAG("map_id", std::to_string(m_map.GetId())));
        MapWorkerTrace::BeginMap(m_map.GetId(), m_map.GetInstanceId());
        m_map.Update(m_diff, s_diff);
        MapWorkerTrace::EndMap();
        m_updater.update_finished();
    }

private:
    Map& m_map;
    MapUpdater& m_updater;
    uint32 m_diff;
    uint32 s_diff;
};

class MapPreloadRequest : public UpdateRequest
{
public:
    MapPreloadRequest(uint32 mapId, MapUpdater& updater)
        : _mapId(mapId), _updater(updater)
    {
    }

    void call() override
    {
        Map* map = sMapMgr->CreateBaseMap(_mapId);
        LOG_INFO("server.loading", ">> Loading All Grids For Map {} ({})", map->GetId(), map->GetMapName());
        map->LoadAllGrids();
        _updater.update_finished();
    }

private:
    uint32 _mapId;
    MapUpdater& _updater;
};

class LFGUpdateRequest : public UpdateRequest
{
public:
    LFGUpdateRequest(MapUpdater& u, uint32 d) : m_updater(u), m_diff(d) {}

    void call() override
    {
        sLFGMgr->Update(m_diff, 1);
        m_updater.update_finished();
    }
private:
    MapUpdater& m_updater;
    uint32 m_diff;
};

MapUpdater::MapUpdater() : pending_requests(0), _cancelationToken(false)
{
}

void MapUpdater::activate(std::size_t num_threads)
{
    _workerThreads.reserve(num_threads);
    for (std::size_t i = 0; i < num_threads; ++i)
    {
        _workerThreads.push_back(std::thread(&MapUpdater::WorkerThread, this));
    }
}

void MapUpdater::deactivate()
{
    _cancelationToken = true;

    wait();  // This is where we wait for tasks to complete

    _queue.Cancel();  // Cancel the queue to prevent further task processing

    // Join all worker threads
    for (auto& thread : _workerThreads)
    {
        if (thread.joinable())
        {
            thread.join();
        }
    }
}

void MapUpdater::wait()
{
    std::unique_lock<std::mutex> guard(_lock);  // Guard lock for safe waiting

    // Wait until there are no pending requests
    _condition.wait(guard, [this] {
        return pending_requests.load(std::memory_order_acquire) == 0;
    });
}

void MapUpdater::schedule_task(UpdateRequest* request)
{
    // Atomic increment for pending_requests
    pending_requests.fetch_add(1, std::memory_order_release);
    _queue.Push(request);
}

void MapUpdater::schedule_update(Map& map, uint32 diff, uint32 s_diff)
{
    schedule_task(new MapUpdateRequest(map, *this, diff, s_diff));
}

void MapUpdater::schedule_map_preload(uint32 mapid)
{
    schedule_task(new MapPreloadRequest(mapid, *this));
}

void MapUpdater::schedule_lfg_update(uint32 diff)
{
    schedule_task(new LFGUpdateRequest(*this, diff));
}

bool MapUpdater::activated()
{
    return !_workerThreads.empty();
}

void MapUpdater::update_finished()
{
    // Atomic decrement for pending_requests
    if (pending_requests.fetch_sub(1, std::memory_order_acquire) == 1)
    {
        // Only notify when pending_requests becomes 0 (i.e., all tasks are finished)
        std::lock_guard<std::mutex> lock(_lock);  // Lock only for condition variable notification
        _condition.notify_all();  // Notify waiting threads that all requests are complete
    }
}

void MapUpdater::WorkerThread()
{
    LoginDatabase.WarnAboutSyncQueries(true);
    CharacterDatabase.WarnAboutSyncQueries(true);
    WorldDatabase.WarnAboutSyncQueries(true);

    while (!_cancelationToken)
    {
        UpdateRequest* request = nullptr;

        _queue.WaitAndPop(request);  // Wait for and pop a request from the queue

        if (!_cancelationToken && request)
        {
            request->call();  // Execute the request
            delete request;  // Clean up after processing
        }
    }
}
