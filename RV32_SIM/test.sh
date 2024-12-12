#!/bin/bash

# 定义输入和输出文件路径
FILES=(
  "SS_RFResult.txt"
  "StateResult_SS.txt"
  "SS_DMEMResult.txt"
  "FS_RFResult.txt"
  # "StateResult_FS.txt"
  "FS_DMEMResult.txt"
)


# 定义目录路径
INPUT_DIR="input"
OUTPUT_DIR="output/testcase2"

# 遍历文件列表并比较
for FILE in "${FILES[@]}"; do
  echo "Comparing $INPUT_DIR/$FILE with $OUTPUT_DIR/$FILE:"
  
  # 判断文件是否相同
  if diff -q "$OUTPUT_DIR/$FILE" "$INPUT_DIR/$FILE" > /dev/null; then
    echo "SAME"
  else
    echo "DIFFERENT"
    # 显示差异并保留 input 文件行号
    diff "$OUTPUT_DIR/$FILE" "$INPUT_DIR/$FILE" \
      | grep -E "^[0-9]+[acd]" \
      | sed -E "s/^([0-9]+)([acd].*)/>> Line \1: \2/"
  fi
  
  echo ""
done
