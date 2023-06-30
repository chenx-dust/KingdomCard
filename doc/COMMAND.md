# 指令文档

## 控制器的指令

### 游戏开始 GAME_START

`PlayerIdentity` 玩家身份

### 游戏状态 GAME_STATUS

`uint8` 总人数

`Player[]` 玩家信息

```
Player = {
    uint8 id
    uint8 hp
    uint8 mp
    uint8 cardCount
}
```

`uint8` 当前回合玩家 id

`uint8` 主公 id

### 他人出牌 NOTICE_CARD

`uint8` 出牌玩家 id

`Card` 出的牌

```
Card = {
    uint8 id,
    CardType type,
}
```

`uint8` 出的牌的目标玩家 id (可选)

`uint8` 偷取的牌 id (可选)

**注意判断目标是否是自己**

**自己打出的牌通过这个指令确认打出**

包括反制牌组

### 濒死求援 NOTICE_DYING

`uint8` 濒死玩家 id

抢先制

**回合当前者可用桃园结义救**

### 死亡通知 NOTICE_DEAD

`uint8` 死亡玩家 id

### 游戏结束 GAME_OVER

`PlayerIdentity` 胜利阵营

### 你的回合 YOUR_TURN

`float16` 你的回合剩余时间

**每次出牌反制后重新发送**

### 新得到卡牌 NEW_CARD

`Card[]` 新得到的卡牌

### 强制弃牌 (回合被迫结束) DISCARD_CARD

`Card[]` 被强制放弃的卡牌

这么写的原因是若玩家超时不结束会强制弃牌

[//]: # (### 弃牌通告 NOTICE_DISCARD)

[//]: # ()
[//]: # (`uint8` 弃牌玩家 id)

[//]: # ()
[//]: # (`Card[]` 弃牌)

## 玩家发出的指令

### 出牌 ACTION_PLAY

`Card` 出的牌

`uint8` 出的牌的目标玩家 id (可选)

`uint8` 拿牌编号 (可选)

### 结束回合(弃牌) ACTION_PASS

`Card[]` 弃掉的牌

**服务端要做校验**

