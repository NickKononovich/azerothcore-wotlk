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

// [jrn20] An appender that hands every log line to a sink installed at runtime, with a millisecond stamp.
//
// WHY THIS EXISTS IN THE CORE AT ALL, and how to take it out again.
// mod-playerbots keeps its history in MySQL (acore_playerbots, pb_journal_*) so that a measurement survives a server
// restart and a count is a query instead of a grep. The raw stream could not join that history from the module side:
//   * every line goes through LOG_INFO("playerbots", ...), and there are thousands of such call sites;
//   * an appender is the only interception point, and appenders are created inside sLog->Initialize() from the
//     config - which runs in Main() long before any module is loaded, and Log has no way to attach one later;
//   * the log FILE carries no timestamp at all (Appender.Playerbots uses flags 0), so loading the file afterwards
//     cannot recover when a line happened, let alone to the millisecond.
// So one registration line in Main.cpp is the whole core-side cost. This file knows nothing about any module: it
// only forwards to whatever sink was installed, and with no sink it does nothing at all.
//
// TO REMOVE: delete the `sLog->RegisterAppender<AppenderJournal>()` line in Main.cpp and these two files. Nothing
// else refers to them, and a server whose config names no appender of type 100 never constructs one.
//
// ACCURACY, stated because it is the kind of thing that quietly invalidates a comparison. The stamp is taken here,
// when the line is written, not when it was logged. With Log.Async.Enable = 0 (the default, and what this server
// runs) writing happens on the calling thread immediately after the call, so the two are microseconds apart. With
// Log.Async.Enable = 1 the write happens later on a strand, and the stamp becomes "when the logger got to it" -
// which is a different quantity under the same name. Turn async logging on and the millisecond column stops meaning
// what it says.

#ifndef APPENDERJOURNAL_H
#define APPENDERJOURNAL_H

#include "Appender.h"
#include <functional>
#include <vector>

class AC_COMMON_API AppenderJournal : public Appender
{
public:
    // 100: outside the core's own appender types (console 1, file 2, db 3), so adding one upstream cannot clash.
    static constexpr AppenderType type = AppenderType(100);

    // atMs: the wall clock in milliseconds. logger: the log filter the line came from ("playerbots"). text: the
    // line itself, without any prefix. Called on the logging thread; a sink must therefore be cheap and must not
    // log anything itself.
    using Sink = std::function<void(uint64 atMs, std::string const& logger, std::string const& text)>;

    AppenderJournal(uint8 id, std::string const& name, LogLevel level, AppenderFlags flags,
                    std::vector<std::string_view> const& args);

    // Installs the sink (an empty function removes it). Lines logged before a sink is installed - everything up to
    // the point the module loads - are dropped, which is why the journal's own history starts at the boot row and
    // not at the first line of the process.
    static void SetSink(Sink sink);

    AppenderType getType() const override { return type; }

private:
    void _write(LogMessage const* message) override;
};

#endif
