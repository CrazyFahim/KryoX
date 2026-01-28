for f in ../examples/t*.loc; do
  echo
  echo "===== Running $f ====="
  ./loc < "$f" || break
done
