# Step2でmakeコマンドが失敗した

下記のエラーが発生：

```sh
$ make
cc -std=c11 -g -static    9cc.c   -o 9cc
/usr/bin/ld: -lc が見つかりません
collect2: エラー: ld はステータス 1 で終了しました
make: *** [9cc] エラー 1
```

## 調査

`/usr/bin/ld: -lc が見つかりません` というメッセージは、 **-l_** にあたるライブラリが見つからないことを示すらしい

[参考：makeで/usr/bin/ld: cannot find -l○○○というエラーが出た時 - Qiita](https://qiita.com/Accent/items/83b6ee7d0b2092060bbb)

インストール済みのライブラリを調べたところ、 `glibc-static` が足りなかった。

```sh
$ yum list installed | grep glibc
glibc.x86_64                        2.17-292.el7                @base           
glibc-common.x86_64                 2.17-292.el7                @base           
glibc-devel.x86_64                  2.17-292.el7                @base           
glibc-headers.x86_64                2.17-292.el7                @base           
```

`yum search glibc-static` で以下の情報を確認。
> glibc-static.x86_64 : C library static libraries for -static linking.

`Makefile` の中で `CFLAGS=-std=c11 -g -static` としていたため、 `-static` オプションのためにこのライブラリが必要となっていた。

## 解決

```sh
$ sudo yum install -y glibc-static
~~~
Complete!
$ make
cc -std=c11 -g -static    9cc.c   -o 9cc
(正常終了)
```

ライブラリインストールで無事解決。
