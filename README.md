# MiLo Bot System Definition (`bot_system_def`)

2台目 MiLo Bot システムで使う、コマンド・ID・コード・パケットの定義と、シーケンス（自動で順番に実行する手順）をまとめたリポジトリです。
すべてのコンポーネントが同じ定義を使うことで、番号の食い違いによる誤動作（停止を送ったつもりが別の動作になる など）を防ぎます。

## 📂 ファイル構成
```
bot_system_def/
├── bot_command_def.h    # コマンド・ID・コード・パケットの定義（enum class）
├── bot_sequence_csv.h   # bot_sequence.csv の名前 → 番号の対応表（CSV管理が使う）
├── bot_sequence.csv     # シーケンス（PREPARE / NORMAL / END）の手順
└── README.md
```

## 📌 役割分担

| | ヘッダーファイル（`bot_command_def.h`） | CSV ファイル（`bot_sequence.csv`） |
|---|---|---|
| 中身 | コマンド・ID・コードと番号の対応 | 自動で順番に実行するコマンドの並び |
| 使う側 | 全コンポーネント（`#include` して組み込む） | CSV管理だけ（起動時に読み込む） |
| 書き方 | 番号を明示して書く | コマンド名で書く（番号は書かない） |
| 変えたとき | 全コンポーネントを再ビルド | CSV を書き換えて CSV管理を再起動 |

番号はヘッダーの 1 か所だけで管理し、手順は変更が多いので CSV で管理します。

## 1️⃣ `bot_command_def.h`

`namespace milo` の中に、次の enum class を定義しています。値はすべて明示しています。

| 名前 | 中身 | 値の例 |
|---|---|---|
| `COMMAND` | コマンド番号（0〜22） | `MOVE = 8`、`PAUSE = 4` |
| `LOCATION_ID` | 地点 ID・待機地点 | `HOME = 101`、`STORE1_WAIT = 202` |
| `SEQUENCE_ID` | シーケンス番号 | `PREPARE = 1`、`NORMAL = 2`、`END = 3` |
| `RUN_MODE` / `POSE_TARGET` / `MAP_SAVE` | コマンドのパラメータの値 | `MANUAL = 1` |
| `TURNTABLE_ACTION` / `CONVEYOR_ACTION` / `CURTAIN_ACTION` | 単体操作（UNIT_OPERATE）の動作番号 | `ROTATE = 1` |
| `ORDER_TYPE` | 指令種別 | `EXECUTE = 0`、`ESTOP = 4` |
| `TARGET_DEVICE` | 対象機器 | `CART = 1`、`CONVEYOR = 3` |
| `RESULT` | 結果 | `RUNNING = 0`、`COMPLETED = 1` |
| `CART_EVENT` | 台車状態のイベント | `PERSON_DETECTED = 1` |
| `BOT_STATE` / `DETAIL_CODE` / `SEQUENCE_STATE` | 状態 | `IDLE = 1` |
| `ERROR_CODE` / `REJECT_REASON` | エラーコード・拒否理由 | `E001_POSE_LOST = 1`、`F001_DIAG_NG = 101` |
| `〇〇_PACKET` | 各パケットの配列インデックス | `CMD_PACKET::TARGET = 2` |

ID の範囲は 1〜99 が Bot、101〜199 が Station・地点、201〜299 が待機地点（対応する地点 ID＋100）です。

### 使い方

```cpp
#include "bot_command_def.h"

// 受け取った番号を名前で判定する
if (cmd == static_cast<int>(milo::COMMAND::PAUSE)) {
    stop_all();
}

// パケットに入れるときは整数に変換する
data[milo::to_int(milo::CMD_PACKET::DEST_ID)] = 1;                        // Bot 1
data[milo::to_int(milo::CMD_PACKET::COMMAND)] = milo::to_int(milo::COMMAND::MOVE);       // 8
data[milo::to_int(milo::CMD_PACKET::TARGET)]  = milo::to_int(milo::LOCATION_ID::STORE1); // 102
```

### コマンドの配列

コマンドは長さ 6 の数値配列で送ります（`CMD_PACKET`）。

| 位置 | 項目 | 内容 |
|---|---|---|
| 0 | 宛先 ID | bot_id・station_id（0＝全台） |
| 1 | コマンド番号 | `COMMAND` |
| 2 | 対象 | 地点 ID・シーケンス番号・地図番号など（なければ 0） |
| 3〜5 | パラメータ | コマンドごと（座標・動作番号など、使わなければ 0） |

例：Bot 1 で通常運転を開始 → `[1, 3, 2, 0, 0, 0]`、Bot 1 を Store 1 へ移動 → `[1, 8, 102, 0, 0, 0]`

コマンド入力（GUI）は、番号を見て送り先を振り分けます（GUI → CSV管理 の「シーケンス指令」も同じ形の配列です）。

| コマンド | 送り先 |
|---|---|
| `START_SEQUENCE`・`RESUME`・`CANCEL` | CSV管理だけ（シーケンス指令） |
| `PAUSE`・`ESTOP_ENABLE` | CSV管理（シーケンス指令）と Task_provider の両方 |
| それ以外 | Task_provider だけ |

### シーケンス状態の配列

CSV管理が GUI へ、Bot ごとに長さ 11 の配列で送ります（`SEQ_STATE_PACKET`）。状態が変わったときと一定周期で送ります。

| 位置 | 項目 | 内容 |
|---|---|---|
| 0 | bot_id | |
| 1 | シーケンス状態 | `SEQUENCE_STATE`（0 なし／1 実行中／2 中断中） |
| 2 | シーケンス番号 | `SEQUENCE_ID`（なしは 0） |
| 3 | ステップ | 実行中・中断中のステップ（1〜、なしは 0） |
| 4 | ステップ数 | そのシーケンスの全ステップ数 |
| 5 | ステップのコマンド | `COMMAND`（なしは -1） |
| 6 | 中断の原因 | ステップの `ERROR_CODE`／`REJECT_REASON`（PAUSE・なしは 0） |
| 7 | 再開可否 | 今 RESUME を受け付けられるか（0 不可／1 可） |
| 8 | 直近のシーケンス指令 | GUI から届いた `COMMAND`（なしは -1） |
| 9 | その結果 | `RESULT`（1 受け付けた／3 拒否） |
| 10 | 拒否理由 | `REJECT_REASON`（受け付けたときは 0） |

## 2️⃣ `bot_sequence.csv`

```csv
sequence,step,command,target,param
PREPARE,1,SELF_DIAG,SELF,
PREPARE,2,LOAD_MAP,1,
PREPARE,3,MECHA_HOME,SELF,
NORMAL,1,RECEIVE,HOME,
NORMAL,2,MOVE,STORE1,
NORMAL,3,EXCHANGE,STORE1,
NORMAL,4,MOVE,HOME,
NORMAL,5,SEND_OUT,HOME,
END,1,MOVE,HOME,
END,2,SHUTDOWN,SELF,
```

| 列 | 内容 |
|---|---|
| `sequence` | `PREPARE` / `NORMAL` / `END` |
| `step` | 1 から始まる通し番号（シーケンスごと） |
| `command` | コマンド名（下の 8 つのみ） |
| `target` | `SELF`（START_SEQUENCE で指定した Bot）、`HOME` / `STORE1` / `STORE2`、または数値（地図番号など） |
| `param` | 予備（今は空欄） |

- 1 行＝1 ステップです。CSV管理が上から 1 行ずつ送り、完了（システム状態の結果＝1）を受けてから次の行を送ります。
- CSV に書けるコマンドは、自動で順番に実行してよい `SELF_DIAG`・`LOAD_MAP`・`MECHA_HOME`・`RECEIVE`・`MOVE`・`EXCHANGE`・`SEND_OUT`・`SHUTDOWN` の 8 つだけです。`ESTOP_ENABLE` などは書いても読み込みません。
- 名前の書き間違いがあれば、CSV管理は起動時にエラーにします。
- 文字コードは UTF-8、区切りはカンマです。

## 3️⃣ `bot_sequence_csv.h`

CSV管理が `bot_sequence.csv` を読み込むときに使う、名前 → 番号の対応表です。

```cpp
#include "bot_sequence_csv.h"

milo::COMMAND cmd;
int target;
if (!milo::csv_command_from_name("MOVE", cmd) ||
    !milo::csv_target_from_name("STORE1", target)) {
    // CSV の書き間違い → 読み込みをやめる
}
```

## ⚠️ 注意事項

- **番号は変えない・使い回さない。** 追加するときは各 enum class の末尾に足します。
- **ヘッダーを変えたら、`#include` している全コンポーネントを再ビルドしてください。** 古いままのコンポーネントがあると番号がずれます。
- **Visual Studio では `/utf-8` オプションが必要です**（日本語コメントを Shift-JIS として読んでしまうため）。
- 既存ヘッダー（`carrier_mode_def.h` など）と名前がぶつからないよう、定義は `namespace milo` の中に置いています。
- 主状態の「エラー」は `ERROR_` です（`ERROR` は Windows のヘッダーでマクロ定義されているため）。
- ROS（Python など C++ 以外）からはこのヘッダーを読めないので、ブリッジRTC で変換するか、同じ値を ROS 側にも定義してください。

## 📌 未定の項目
- `SET_PARAM` で変更できる項目の番号
- Station（Home・Store）の単体操作の動作番号
- 機構のステータスの数（`STATION_STATE_PACKET`）
- ポートのデータ型（座標に負の値・32 m 超を使うため TimedLongSeq を推奨）

## 📌 貢献方法
定義を追加・変更する場合は、ブランチを作って変更し、Pull Request を作成してください。番号を変更する場合は、影響するコンポーネントを PR に書いてください。
