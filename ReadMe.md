# 说明

这是一个用于HT5027的Flash编程算法。

# 注意事项

- 由于看门狗问题,此编程算法必须在TEST=0且JTAGWDTEN （PA6）=0时使用。
- 由于HT5027比较坑的Flash读保护机制,若编程后Flash的0xFC1地址不为0xFF，将有可能校验失败。

