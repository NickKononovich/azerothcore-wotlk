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

#include "AppenderJournal.h"
#include "LogMessage.h"
#include <chrono>
#include <mutex>

namespace
{
    // [jrn20] Guarded because the sink is installed once from the loading thread while other threads may already be
    // logging. A shared_mutex would be cheaper on paper, but the sink is read once per log line and the lines are
    // written one at a time anyway.
    std::mutex g_sinkLock;
    AppenderJournal::Sink g_sink;
}

AppenderJournal::AppenderJournal(uint8 id, std::string const& name, LogLevel level, AppenderFlags flags,
    std::vector<std::string_view> const& /*args*/)
    : Appender(id, name, level, flags)
{
}

void AppenderJournal::SetSink(Sink sink)
{
    std::lock_guard<std::mutex> lock(g_sinkLock);
    g_sink = std::move(sink);
}

void AppenderJournal::_write(LogMessage const* message)
{
    Sink sink;
    {
        std::lock_guard<std::mutex> lock(g_sinkLock);
        if (!g_sink)
            return;
        sink = g_sink;
    }

    // The stamp is taken here, not from message->mtime: mtime is a Seconds, and the whole point of the journal is
    // that two things inside one second can be told apart and put in order.
    uint64 const atMs = static_cast<uint64>(std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count());
    sink(atMs, message->type, message->text);
}
