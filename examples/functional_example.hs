
my_map _ [] = []
my_map f lst =
  let h:t = lst in
  f h : my_map f t

g x = x + 1

main =
  let myList = [1,2,3,4] in
  let newList = my_map g myList in
  putStrLn (show myList)
