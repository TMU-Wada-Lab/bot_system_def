#ifndef BOT_SEQUENCE_CSV_H
#define BOT_SEQUENCE_CSV_H

// =============================================================================
// シーケンス CSV（bot_sequence.csv）の読み込み用：名前 → 番号の対応表
//
//  ・CSV にはコマンド・地点・シーケンスを「名前」で書く（番号は書かない）。
//    CSV管理はこの表で名前を bot_command_def.h の enum に変換する。
//    番号が変わっても CSV は書き換え不要。
//  ・この表にない名前があれば、CSV管理は起動時にエラーにして読み込みをやめる。
//  ・CSV に書けるコマンドは下の表にあるものだけ（自動で順番に実行してよいもの）。
//    ESTOP や SET_MODE などは表に入れないので、CSV に書いても読み込めない。
//
// CSV の形式（1 行目は見出し、UTF-8、カンマ区切り）
//   sequence,step,command,target,param
//   sequence：PREPARE / NORMAL / END
//   step    ：1 から始まる通し番号（シーケンスごと）
//   command ：下の CSV_COMMANDS の名前
//   target  ：下の CSV_TARGETS の名前、または数値（地図番号など）
//   param   ：予備（今は空欄）
// =============================================================================

#include <cstring>
#include <cstdlib>
#include "bot_command_def.h"

namespace milo {

// CSV の列
enum class SEQ_CSV_COLUMN {
    SEQUENCE     = 0,
    STEP         = 1,
    COMMAND      = 2,
    TARGET       = 3,
    PARAM        = 4,
    DATA_LENGTH  = 5
};

// CSV に書けるコマンド
struct CsvCommandName {
    const char* name;
    COMMAND     command;
};

static const CsvCommandName CSV_COMMANDS[] = {
    { "SELF_DIAG",  COMMAND::SELF_DIAG  },
    { "LOAD_MAP",   COMMAND::LOAD_MAP   },
    { "MECHA_HOME", COMMAND::MECHA_HOME },
    { "RECEIVE",    COMMAND::RECEIVE    },
    { "MOVE",       COMMAND::MOVE       },
    { "EXCHANGE",   COMMAND::EXCHANGE   },
    { "SEND_OUT",   COMMAND::SEND_OUT   },
    { "SHUTDOWN",   COMMAND::SHUTDOWN   },
};

// target 列に書ける名前（SELF＝自分の Bot、宛先は START_SEQUENCE の bot_id）
struct CsvTargetName {
    const char* name;
    int         id;
};

static const CsvTargetName CSV_TARGETS[] = {
    { "SELF",   0 },
    { "HOME",   to_int(LOCATION_ID::HOME)   },
    { "STORE1", to_int(LOCATION_ID::STORE1) },
    { "STORE2", to_int(LOCATION_ID::STORE2) },
};

// sequence 列に書ける名前
struct CsvSequenceName {
    const char* name;
    SEQUENCE_ID id;
};

static const CsvSequenceName CSV_SEQUENCES[] = {
    { "PREPARE", SEQUENCE_ID::PREPARE },
    { "NORMAL",  SEQUENCE_ID::NORMAL  },
    { "END",     SEQUENCE_ID::END     },
};

// 名前 → コマンド。見つからなければ false（CSV の書き間違い）。
inline bool csv_command_from_name(const char* name, COMMAND& out) {
    for (const auto& c : CSV_COMMANDS) {
        if (std::strcmp(c.name, name) == 0) { out = c.command; return true; }
    }
    return false;
}

// 名前または数値 → target の番号。見つからなければ false。
inline bool csv_target_from_name(const char* name, int& out) {
    for (const auto& t : CSV_TARGETS) {
        if (std::strcmp(t.name, name) == 0) { out = t.id; return true; }
    }
    // 数値（地図番号など）
    char* end = nullptr;
    const long v = std::strtol(name, &end, 10);
    if (end != name && *end == '\0') { out = static_cast<int>(v); return true; }
    return false;
}

// 名前 → シーケンス番号。見つからなければ false。
inline bool csv_sequence_from_name(const char* name, SEQUENCE_ID& out) {
    for (const auto& s : CSV_SEQUENCES) {
        if (std::strcmp(s.name, name) == 0) { out = s.id; return true; }
    }
    return false;
}

}  // namespace milo

#endif  // BOT_SEQUENCE_CSV_H
