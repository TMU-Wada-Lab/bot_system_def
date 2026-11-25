#ifndef BOT_COMMAND_DEF_H
#define BOT_COMMAND_DEF_H

// =============================================================================
// 2台目 MiLo Bot のコマンド・ID・コード・パケット定義
//
//  ・全コンポーネント（CSV管理・コマンド入力(GUI)・Task_provider・Bot_controller・
//    Station・ブリッジRTC）が、このファイルを #include して同じ番号を使う。
//  ・通信で送る番号なので、値はすべて明示している。
//    追加するときは末尾に足し、既存の番号は変えない・使い回さない。
//  ・既存ヘッダー（carrier_mode_def.h の SYSTEM_STATE など）と名前がぶつからないよう、
//    namespace milo の中に置く。使うときは milo::COMMAND::PAUSE のように書く。
//  ・パケットに入れるときは milo::to_int(...) で整数に変換する。
// =============================================================================

namespace milo {

// enum class → 整数（パケットに詰めるとき用）
template <typename E>
constexpr int to_int(E e) { return static_cast<int>(e); }

// -----------------------------------------------------------------------------
// コマンド番号（コマンド一覧 p.13〜15）
// -----------------------------------------------------------------------------
enum class COMMAND {
    // 準備
    SELF_DIAG        = 0,   // 自己診断
    LOAD_MAP         = 1,   // 地図読み込み（初期位置＝ホーム）
    MECHA_HOME       = 2,   // 機構の原点出し
    // 運転（CSV管理が受け取る）
    START_SEQUENCE   = 3,   // シーケンス開始
    PAUSE            = 4,   // 一時停止（CSV管理と Task_provider の両方が受け取る）
    RESUME           = 5,   // 再開
    CANCEL           = 6,   // 中止
    // ステップ（CSV にも書ける）
    RECEIVE          = 7,   // Home 授受機構からコンテナ受け取り
    MOVE             = 8,   // 自律移動＋授受機構への位置合わせ
    EXCHANGE         = 9,   // Store でコンテナ交換
    SEND_OUT         = 10,  // Home 授受機構へコンテナ送り出し
    // 復旧
    RETREAT          = 11,  // 退避
    CLEAR_ERROR      = 12,  // エラー解除
    RESET_POSE       = 13,  // 自己位置の設定し直し
    // 安全
    ESTOP_ENABLE     = 14,  // 非常停止（最優先）
    ESTOP_DISENABLE  = 15,  // 非常停止解除
    // 保守
    SET_MODE         = 16,  // AUTO / MANUAL 切り替え
    UNIT_OPERATE     = 17,  // 機構の単体操作
    SET_PARAM        = 18,  // パラメータ変更
    MAP_START        = 19,  // 地図作成開始
    MAP_END          = 20,  // 地図作成終了
    // 終了
    SHUTDOWN         = 21,  // シャットダウン
    // 保守（追加）
    RELEASE_STATION  = 22,  // 地点の予約を手動で外す
    NUM              = 23   // 個数（番号ではない）
};

// -----------------------------------------------------------------------------
// ID（IDの振り分け方 p.17）
//   1〜99：Bot、101〜199：Station・地点、201〜299：待機地点（対応する地点ID＋100）
// -----------------------------------------------------------------------------
constexpr int BOT_ID_ALL = 0;   // 全 Bot（ESTOP のときだけ全 Station も含む）
constexpr int BOT_ID_MIN = 1;
constexpr int BOT_ID_MAX = 99;

// 地点 ID（Station の station_id と同じ番号）
enum class LOCATION_ID {
    NONE         = 0,    // 対象なし
    HOME         = 101,  // MiLo Home
    STORE1       = 102,  // MiLo Store 1
    STORE2       = 103,  // MiLo Store 2
    STORE1_WAIT  = 202,  // STORE1 待機地点（HOME は複数台入れるので待機地点なし）
    STORE2_WAIT  = 203   // STORE2 待機地点
};

// シーケンス番号（CSV の sequence 列の名前と対応）
enum class SEQUENCE_ID {
    NONE     = 0,
    PREPARE  = 1,  // 起動・準備
    NORMAL   = 2,  // 通常運転
    END      = 3   // 終了
};

// 地図番号は 1〜 を地図表（番号 → ファイル名）で管理する。0 は「なし」。
constexpr int MAP_ID_NONE = 0;

// -----------------------------------------------------------------------------
// コマンドのパラメータで使う値
// -----------------------------------------------------------------------------
// SET_MODE の対象
enum class RUN_MODE {
    AUTO    = 0,
    MANUAL  = 1
};

// RESET_POSE の対象（COORDINATE のときはパラメータに x[mm], y[mm], θ[mrad]）
enum class POSE_TARGET {
    HOME        = 0,
    COORDINATE  = 1
};

// MAP_END の対象
enum class MAP_SAVE {
    DISCARD  = 0,  // 保存しない
    SAVE     = 1   // 保存する（パラメータに地図番号）
};

// UNIT_OPERATE の動作（対象は TARGET_DEVICE）
enum class TURNTABLE_ACTION {
    HOMING  = 0,   // 原点出し
    ROTATE  = 1    // 回転（パラメータに角度 deg）
};

enum class CONVEYOR_ACTION {
    STOP      = 0,
    INTAKE    = 1, // 取り込み方向（パラメータに速度 mm/sec・時間 msec）
    SEND_OUT  = 2  // 送り出し方向（同上）
};

enum class CURTAIN_ACTION {
    HOMING  = 0,   // 原点出し
    OPEN    = 1,   // 上昇（開く位置は SET_PARAM で設定）
    CLOSE   = 2    // 下降
};

// -----------------------------------------------------------------------------
// パケット内で使うコード（p.31）
// -----------------------------------------------------------------------------
// 指令種別（Bot指令・Station指令・台車指令）
enum class ORDER_TYPE {
    EXECUTE        = 0,  // 実行（制御指令・移動）
    PAUSE          = 1,  // 一時停止
    RESUME         = 2,  // 再開
    CANCEL         = 3,  // 中止
    ESTOP          = 4,  // 非常停止
    ESTOP_RELEASE  = 5,  // 非常停止解除
    SET_POSE       = 6,  // 自己位置設定
    MAP_START      = 7,  // 地図作成開始
    MAP_END        = 8   // 地図作成終了
};

// 対象機器（Bot）
enum class TARGET_DEVICE {
    BOT_ALL    = 0,  // Bot 全体
    CART       = 1,  // 移動台車
    TURNTABLE  = 2,  // ターンテーブル
    CONVEYOR   = 3,  // ベルトコンベア
    CURTAIN    = 4   // ロールカーテン
};

// 結果
enum class RESULT {
    RUNNING    = 0,  // 実行中
    COMPLETED  = 1,  // 完了
    FAILED     = 2,  // 失敗（エラーコードを参照）
    REJECTED   = 3,  // 拒否（エラーコード欄に REJECT_REASON を入れる）
    ABORTED    = 4   // 中断（一時停止・非常停止）
};

// 台車状態のイベント
enum class CART_EVENT {
    NONE             = 0,
    PERSON_DETECTED  = 1,  // 人物検出
    PURCHASE_DONE    = 2   // 購入完了
};

// -----------------------------------------------------------------------------
// 状態
// -----------------------------------------------------------------------------
// Bot の主状態（システム状態・Bot状態）
//   ERROR は Windows のヘッダーでマクロ定義されているため ERROR_ とする
//   （マクロは namespace や enum class のスコープを無視して置き換わるため）
enum class BOT_STATE {
    NOT_READY      = 0,  // 未準備
    IDLE           = 1,  // 待機
    RUNNING        = 2,  // 実行中
    RETREATING     = 3,  // 退避中
    ERROR_         = 4,  // エラー（再開可否はシステム状態の「再開可否」）
    FAULT          = 5,  // 故障
    ESTOP          = 6,  // 非常停止
    MANUAL         = 7,  // 手動
    SHUTTING_DOWN  = 8   // 終了処理中
};

// 詳細コード（主状態の中の段階）
enum class DETAIL_CODE {
    NONE                 = 0,
    WAITING_FOR_SLOT     = 1,  // 空き待ち（待機地点）
    STOPPED_BY_PERSON    = 2,  // 人物検出で停止中
    WAITING_PURCHASE     = 3,  // 購入待ち
    AVOIDING_OBSTACLE    = 4,  // 障害物回避中
    RETRYING             = 5,  // 授受機構の再探索中
    MAPPING              = 6,  // 地図作成中（MANUAL）
    MANUAL_FROM_FAULT    = 7   // 故障から手動へ入った（AUTO に戻れない）
};

// シーケンスの状態（CSV管理が持つ）
enum class SEQUENCE_STATE {
    NONE       = 0,  // なし
    RUNNING    = 1,  // 実行中
    SUSPENDED  = 2   // 中断中
};

// エラーコード（E：エラー、F：故障、C：状態が変わらない失敗）
enum class ERROR_CODE {
    NONE                   = 0,
    E001_POSE_LOST         = 1,    // 自己位置喪失（再開可）
    E002_STATION_NOT_FOUND = 2,    // 授受機構の検知失敗（再開可、MOVE からやり直す）
    E003_EXCHANGE_FAILED   = 3,    // コンテナ交換失敗（再開不可）
    E004_STEP_TIMEOUT      = 4,    // ステップタイムアウト（再開不可）
    E005_COMM_LOST         = 5,    // 通信断（再開不可）
    E006_RETREAT_FAILED    = 6,    // 退避失敗（再開不可）
    E007_WAIT_TIMEOUT      = 7,    // 空き待ちタイムアウト（再開可）
    E008_TRANSFER_FAILED   = 8,    // 受け取り・送り出し失敗（はみ出しなし＝可／あり＝不可）
    F001_DIAG_NG           = 101,  // 診断NG
    F002_HOMING_FAILED     = 102,  // 原点出し失敗
    F003_DRIVE_FAULT       = 103,  // 走行系の故障
    F004_NON_DRIVE_FAULT   = 104,  // 走行系以外の故障
    C001_MAP_LOAD_FAILED   = 201,  // 地図読込失敗
    C002_POSE_NOT_CONVERGED= 202,  // 自己位置が収束しない
    C003_SLAM_START_FAILED = 203,  // SLAM 起動失敗
    C004_MAP_SAVE_FAILED   = 204,  // 地図の保存失敗
    C005_UNIT_FAILED       = 205   // 単体操作の機構エラー
};

// 拒否理由（RESULT::REJECTED のとき、エラーコード欄に入れる）
enum class REJECT_REASON {
    INVALID_STATE    = 301,  // 今の状態では受け付けない
    INVALID_PARAM    = 302,  // パラメータが範囲外・存在しない
    BUSY             = 303,  // ほかのコマンドを実行中
    NOT_READY        = 304,  // 準備未完了
    NOT_RESOLVED     = 305,  // エラーの原因が解消していない
    HW_ESTOP_ACTIVE  = 306,  // HW 非常停止ボタンが押されたまま
    NOT_AT_START     = 307,  // シーケンスの開始条件を満たしていない
    STATION_BUSY     = 308   // 対象の Station を別の Bot が使用中
};

// -----------------------------------------------------------------------------
// パケットの配列インデックス（p.26〜30）。DATA_LENGTH は配列の長さ。
// -----------------------------------------------------------------------------
// CSV管理・コマンド入力 > Task_provider（コマンド）
// コマンド入力 > CSV管理（シーケンス指令）も同じ形式。
//   GUI は START_SEQUENCE・RESUME・CANCEL を CSV管理だけへ、
//   PAUSE・ESTOP_ENABLE を CSV管理と Task_provider の両方へ送る。
enum class CMD_PACKET {
    DEST_ID      = 0,  // 宛先 ID（bot_id・station_id、0＝全台）
    COMMAND      = 1,  // COMMAND
    TARGET       = 2,  // 地点 ID・シーケンス番号・地図番号など（なければ 0）
    PARAM_1      = 3,  // パラメータ（コマンドごと）
    PARAM_2      = 4,
    PARAM_3      = 5,
    DATA_LENGTH  = 6
};

// Task_provider > CSV管理・コマンド入力（システム状態）
enum class SYS_STATE_PACKET {
    BOT_ID             = 0,
    BOT_STATE          = 1,  // BOT_STATE
    RUNNING_COMMAND    = 2,  // COMMAND
    RESULT             = 3,  // RESULT
    ERROR_CODE         = 4,  // ERROR_CODE または REJECT_REASON
    RESUMABLE          = 5,  // 0 不可／1 可
    POS_X              = 6,  // mm
    POS_Y              = 7,  // mm
    POS_THETA          = 8,  // mrad
    RESERVED_LOCATION  = 9,  // LOCATION_ID
    DATA_LENGTH        = 10
};

// CSV管理 > コマンド入力（シーケンス状態）
//   Bot ごとに 1 配列。状態が変わったときと一定周期で送る。
//   LAST_REQUEST〜REJECT_REASON は、GUI から届いた直近のシーケンス指令
//   （START_SEQUENCE・PAUSE・RESUME・CANCEL・ESTOP_ENABLE）を受け付けたかどうか。
enum class SEQ_STATE_PACKET {
    BOT_ID         = 0,
    SEQ_STATE      = 1,  // SEQUENCE_STATE
    SEQUENCE_ID    = 2,  // SEQUENCE_ID（なしは 0）
    STEP           = 3,  // 実行中・中断中のステップ番号（1〜、なしは 0）
    STEP_COUNT     = 4,  // そのシーケンスのステップ数
    STEP_COMMAND   = 5,  // そのステップのコマンド（COMMAND、なしは -1）
    STOP_CODE      = 6,  // 中断の原因：ステップの ERROR_CODE／REJECT_REASON（PAUSE・なしは 0）
    RESUMABLE      = 7,  // 今 RESUME を受け付けられるか（0 不可／1 可）
    LAST_REQUEST   = 8,  // 直近のシーケンス指令（COMMAND、なしは -1）
    LAST_RESULT    = 9,  // RESULT：COMPLETED＝受け付けた／REJECTED＝拒否
    REJECT_REASON  = 10, // 拒否したときの REJECT_REASON（受け付けたときは 0）
    DATA_LENGTH    = 11
};

// Task_provider > Bot_controller（Bot指令）。Station指令も同じ形式。
enum class BOT_ORDER_PACKET {
    DEST_ID        = 0,  // bot_id（Station指令なら station_id）
    TARGET_DEVICE  = 1,  // TARGET_DEVICE（Station指令なら機構の番号）
    ORDER_TYPE     = 2,  // ORDER_TYPE
    DATA_START     = 3,  // ここから I2C 制御指令 0〜13、または目的地 ID・x, y, θ
    DATA_LENGTH    = 17  // 3 ＋ I2C 制御指令 14 個
};

// Bot_controller > Task_provider（Bot状態）
enum class BOT_STATE_PACKET {
    BOT_ID            = 0,
    MAIN_STATE        = 1,   // BOT_STATE
    DETAIL_CODE       = 2,   // DETAIL_CODE
    RESULT            = 3,   // RESULT
    ERROR_CODE        = 4,   // ERROR_CODE
    POS_X             = 5,   // mm
    POS_Y             = 6,   // mm
    POS_THETA         = 7,   // mrad
    LAST_LOCATION     = 8,   // 直近の地点 ID
    CART_STATUS       = 9,
    TURNTABLE_STATUS  = 10,
    CONVEYOR_STATUS   = 11,
    CURTAIN_STATUS    = 12,
    CONTAINER_LOADED  = 13,  // 0 無／1 有
    DATA_LENGTH       = 14
};

// Bot_controller > 完了判定（制御指令・上物）
enum class UPPER_ORDER_PACKET {
    MCU_ADDR     = 0,
    I2C_START    = 1,   // ここから I2C 制御指令 0〜13 をそのまま入れる
    DATA_LENGTH  = 15
};

// 完了判定 > Bot_controller（上物状態）
enum class UPPER_STATE_PACKET {
    RESULT       = 0,   // RESULT
    I2C_START    = 1,   // ここから I2C 内部状態 0〜8 をそのまま入れる
    DATA_LENGTH  = 10
};

// Bot_controller > ROS（台車指令）
enum class CART_ORDER_PACKET {
    ORDER_TYPE   = 0,  // ORDER_TYPE
    LOCATION_ID  = 1,  // 目的地 ID
    POS_X        = 2,  // mm（自己位置を設定し直すときだけ使う）
    POS_Y        = 3,  // mm
    POS_THETA    = 4,  // mrad
    DATA_LENGTH  = 5
};

// ROS > Bot_controller（台車状態）
enum class CART_STATE_PACKET {
    CART_STATUS    = 0,
    RESULT         = 1,  // RESULT
    ERROR_CODE     = 2,  // ERROR_CODE
    POS_X          = 3,  // mm
    POS_Y          = 4,  // mm
    POS_THETA      = 5,  // mrad
    LAST_LOCATION  = 6,  // 直近の地点 ID
    EVENT          = 7,  // CART_EVENT
    DATA_LENGTH    = 8
};

// Station > Task_provider（Station状態）。機構のステータスの数は Station ごとに未定。
enum class STATION_STATE_PACKET {
    STATION_ID    = 0,
    MAIN_STATE    = 1,  // BOT_STATE と同じ値
    RESULT        = 2,  // RESULT
    ERROR_CODE    = 3,  // ERROR_CODE
    STATUS_START  = 4   // ここから機構の I2C ステータス
};

}  // namespace milo

#endif  // BOT_COMMAND_DEF_H
