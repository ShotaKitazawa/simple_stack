# About

AF_PACKETを用いたユーザ空間プロトコルスタックをDPDKに対応させる

# Reference

* ソースコードで体感するネットワークの仕組み ～手を動かしながら基礎からTCP/IPの実装までがわかる
    * http://gihyo.jp/book/2018/978-4-7741-9744-9

# Execution

## AF_PACKET

Myeth.ini の作成

```
cat << _EOF_ > Myeth.ini
IP-TTL=64
MTU=1500
MSS=1460
device=ens161
vmac=02:00:00:00:00:01
vip=0.0.0.0
vmask=0.0.0.0
gateway=0.0.0.0
DhcpRequestLeaseTime=600
_EOF_
```

コンパイル, 実行

```
make
./Myeth
```

## DPDK

TODO
