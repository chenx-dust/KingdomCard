# 控制器执行流程

1. 等待玩家上线
2. 开始游戏 GAME_START
3. 进入回合

## 回合流程

1. 公布状态 GAME_STATUS
2. 发牌等待玩家出牌 YOUR_TURN NEW_CARD
3. 根据玩家操作
    - 玩家不出牌，超时，发送被迫结束 TURN_END
    - 玩家出牌，接收到出牌 ACTION_PLAY
        - 对可作用玩家发送 NOTICE_REACT
        - 对不可作用玩家发送 NOTICE_NONE
    - 玩家反制，接收到反制 ACTION_PLAY
        - 对可作用玩家发送 NOTICE_REACT
        - 对不可作用玩家发送 NOTICE_NONE
    - 直到没有反制为止
    - 出现濒死状态，发送濒死求援 NOTICE_DYING
        - 如果有人救，收到 ACTION_PLAY ，发送 NOTICE_NONE
        - 没人救则死亡，发送 NOTICE_DEAD
    - 玩家结束回合 ACTION_PASS ，进入下一玩家
4. 判定是否达到结束条件，达到则发送 GAME_OVER

## 游戏结束条件

1. 主公死亡且有反贼存活，反贼胜利
2. 主公死亡且无反贼存活，内奸胜利
3. 主公存活且反贼死亡，主公胜利
