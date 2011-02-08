
open Dll;;

type element_info = {
	mutable c : int;
  mutable n : int Dllist.node_t 
};;

let dummy_int = Dllist.create 0;;

class smartSet capacity = object(self)

	val mutable _list = None;
	val mutable _count = Array.init capacity (fun i -> { c = 0; n = dummy_int });
	
	method add x =
		_count.(x).c <- _count.(x).c + 1;
		if _count.(x).c == 1 then (
			_list <- Dll.prepend _list x;
			_count.(x).n <- Dll.getn _list;
		)

	method remove x =
(*		if _count.(x).c == 0 then
			failwith "smartSet: attempt to remove an element which is not in the set";*)
		_count.(x).c <- _count.(x).c - 1;
		if _count.(x).c == 0 then (
			_list <- Dll.removeNode _list _count.(x).n;
			_count.(x).n <- dummy_int;
		)

  method iter f = Dll.iter f _list

	method contains x = (_count.(x).c > 0)
	
	method count x = _count.(x).c;

  method states = Dll.to_list _list;
	
	method size = Dll.size _list;
	
	method clear =
		_list <- None;
		for i = 0 to Array.length _count - 1 do
	    _count.(i).c <- 0;	
	    _count.(i).n <- dummy_int;
		done;

	method dump =
		Dll.iter (fun x -> Printf.printf "%d " x) _list;
		Printf.printf "\n"

end;;
