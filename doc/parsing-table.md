
この文書ではlr1ccが出力する構文解析表について記述します。

## 例

```csv
,end,a,b,S
1,Rno-more-ab,S3,,G2
2,A,,,
3,,S5,Rno-more-ab,G4
4,,,S6,
5,,S5,Rno-more-ab,G7
6,Rmore-ab,,,
7,,,S8,
8,,,Rmore-ab,
```

## 構文解析表

構文解析表は状態と記号の組にアクションを対応させるデータ構造です。アクションは次のいずれかです。

- 受容
- 還元
- シフト
- GOTO
- 拒否

構文解析表はRFC 4180準拠なヘッダー付きCSV形式で記述されます。１行目 (ヘッダー) には記号が列挙され、１列目には状態の名前が列挙されます。

各セルには列ヘッダーと行ヘッダーに対応するアクションを表す文字列が格納されます。アクションは次のような形式で記述されます。

| アクション | 形式 |
|:-|:-|
| 受容 | `A` |
| 還元 | `Rproduction-name` |
| シフト | `Sstate-name` |
| GOTO | `Gstate-name` |
| 拒否 | `` |

## 構文解析アルゴリズム

構文解析は次のように行われます。

1. StateStackとTreeStackを空のスタックに初期化する。
2. StateStackに1をプッシュする。
3. StateStackの先頭と入力記号列の先頭からアクションを求め、実行する。
4. 3に戻る。

アクションの実行は次のように定義されます。

| アクション | 手続き |
|:-|:-|
| 受容 | TreeStackの先頭を出力し、構文解析を終了する。 |
| 還元 | StateStackを `len(rhs)` 回ポップする。TreeStackの先頭 `len(rhs)` 個の項目から構文木を作成し、置き換える。StateStackの先頭と `lhs` からアクションを求め、実行する。 |
| シフト | 指定された状態をStateStackにプッシュする。入力記号列の先頭を除去し、TreeStackにプッシュする。 |
| GOTO | 指定された状態をStateStackにプッシュする。 |
| 拒否 | エラーを報告し、構文解析を終了する。 |