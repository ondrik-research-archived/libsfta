open Interim

module OrderedCouple = struct
	type t = int * int
	let compare = compare 
end;;

module OrderedInt = struct
	type t = int
	let compare = compare 
end;;

module CoupleSet = Set.Make(OrderedCouple)
module IntSet = Set.Make(OrderedInt)

let table a = 
	let t = Array.make (sn a) (IntSet.empty,CoupleSet.empty) in 
	let process_rule {r_rhs = r;r_lhs = lhs;r_symbol = a} = 
		match lhs with 
		[||] -> ()
		| [|q|] -> t.(q) <- IntSet.add a (fst t.(q)),CoupleSet.add (a,r) (snd t.(q))
		| _ -> failwith"Localsim.table: not a word automaton"
	in
	List.iter process_rule a.a_rules;
	t

let simulation a =
	let t = table a in
	let n = sn a in
	Array.init n (fun q ->
		Array.init n (fun r ->
			IntSet.subset (fst t.(q)) (fst t.(r)) && CoupleSet.subset (snd t.(q)) (snd t.(r))	
(*			print_newline();*)
(*			(print_int 1;false) && (print_int 2;true)	*)
		)
	)


