#!/bin/bash
assert() {
  expected="$1"
  input="$2"

  ./build/drive/ccompiler "$input" > tmp.s || exit
  gcc -static -o tmp tmp.s
  ./tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$input => $expected expected, but got $actual"
    exit 1
  fi
}

assert 41 "./1.cpp"

echo OK