# lr1cc

lr1ccはLR(1)構文解析表を計算します。

## インストール

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
cmake --install build --prefix /some/where
```

`/some/where` は典型的には `/usr/local` や `"$HOME/.local"` などです。

## 使い方

```sh
lr1cc [-o outfile] [-h] infile
```

入力ファイルの仕様については [grammar-def.md](/doc/grammar-def.md) を参照してください。

出力ファイルの使用については [parsing-table.md](/doc/parsing-table.md) を参照してください。
