open Interim

let compare_blocks b1 b2 = compare (List.length b1) (List.length b2);;
let sort_blocks = List.fast_sort compare_blocks 

let print_blocks_indices blocks =
	let block_to_string block = 
		Printf.sprintf "{%s}" 
			(String.concat " " (List.map (fun q -> string_of_int q) block)) 
	in
	Printf.printf "%s\n" (String.concat " " (List.map block_to_string blocks))

(*	this thinkg keeps a list of blocks sorted by number of elements. Each time
 *	it picks the head block and tries to merge it with a block on the right. If
 *	the block can not be added to any block, it is excluded from further
 *	computation*)
let get_some_blocks rel =
	let rec merge (active,finished) =
		match active with
	  	[] -> finished
		| b::act_tail-> 
			
			let rec add_to_somebody b pre post =
				match post with
				[] -> (sort_blocks pre),(b::finished)
				| h::t -> 
					if Rel.blocks_in_rel rel (h,b) && Rel.blocks_in_rel rel (b,h) then
						(sort_blocks (pre@((b@h)::t))),finished
					else
						add_to_somebody b (pre@[h]) t
			in

			merge (add_to_somebody b [] act_tail)
	in
	merge ((Array.to_list (Array.init (Array.length rel) (fun i -> [i]))),[])
;;

(*Throws an exception if some state of b can not be vanished*)
let vanish_block rel blocks b = 
	let blocks_without_b = List.filter (fun c -> c <> b) blocks in
	let vanish_state blocks q =
		let acceptor = List.find 
			(fun a -> Rel.blocks_in_rel rel (a,[q]) && Rel.blocks_in_rel rel ([q],a)) blocks
		in
		sort_blocks ((q::acceptor)::(List.filter (fun c -> c <> acceptor) blocks))
	in
	List.fold_left vanish_state blocks_without_b b
;;

let rec try_to_vanish_something rel blocks not_tried = match blocks with [] -> [] | _ -> (
	match not_tried with [] -> blocks | hd::tl -> (
		try 
			let new_blocks = vanish_block rel blocks hd in
			try_to_vanish_something rel new_blocks new_blocks
		with _ ->
			try_to_vanish_something rel blocks tl
	)
)

let get_another_blocks rel =
	let rec vanish blocks =
		let new_blocks = try_to_vanish_something rel blocks blocks in
		if (List.length blocks <> List.length new_blocks) then
			vanish new_blocks
		else
			blocks
	in
	vanish (Array.to_list (Array.init (Array.length rel) (fun i -> [i])))


	



	
(*-----------------------------------------------------------------------*)
(*-----------------------------------------------------------------------*)
(*-----------------------------------------------------------------------*)



let get_equivalence_blocks preorder = 
	let members = Array.mapi (fun i _ -> i) preorder in
	let rec get_blocks rest_of_members blocks =
		match rest_of_members with
		[] -> blocks
		| x:: _ ->
			let newblock,rest_of_rest = List.partition (fun y ->
				preorder.(x).(y)&&preorder.(y).(x)) rest_of_members
			in
			get_blocks rest_of_rest (newblock::blocks)
	in
	get_blocks (Array.to_list members) []

let print_relation_on_blocks preorder blocks states =
	let block_to_string block =
		Printf.sprintf "{%s}" (String.concat " " (List.map (fun q -> states.(q).s_name) block)) 
	in
	let above block1 block2 = preorder.(List.hd block1).(List.hd block2) in
	List.iter (function block -> 
		let above_list = List.filter (above block) blocks in
		Printf.printf "%s < %s\n" (block_to_string block) 
			(String.concat " " (List.map block_to_string above_list))
	) blocks 


let print_blocks blocks states =
	let block_to_string block = 
		Printf.sprintf "{%s}" (String.concat " " (List.map (fun q -> states.(q).s_name) block)) 
	in
	Printf.printf "%s\n" (String.concat " " (List.map block_to_string blocks))


let get_representants blocks states =
(*	let blocks = List.rev blocks in*)
	let representants = ref [] in
	let represented_by = Array.make (Array.length states) (-1) in
	let process_block i block =
		let {s_name=name;s_final=final} = states.( 
			try List.find (fun q -> states.(q).s_final) block
			with Not_found -> List.hd block )
		in
		representants:={s_name=name;s_final=final;s_index=i}::!representants;
		List.iter (function q -> represented_by.(q) <- i) block
	in
(*	let _ = List.fold_left (fun i block -> process_block i block; i+1) 0 blocks in*)
	Array.iteri  (fun i block -> process_block i block) (Array.of_list blocks);
	Array.of_list (List.rev !representants),represented_by


let merge_rules rules repre_states represented_by =
	let get_repre_lhs lhs = 
		Array.map (function q -> represented_by.(q)) lhs
	in
	let get_repre_rhs q = represented_by.(q) in
	let with_duplicities = List.map (function {r_rhs=rhs;r_lhs=lhs;r_symbol=symbol} ->
		{r_lhs=get_repre_lhs lhs; r_rhs=get_repre_rhs rhs; r_symbol=symbol}
	) rules in
	let splited = sort_and_split compare with_duplicities in
	List.map (fun rl -> List.hd rl) splited

let merge aut preorder =
	let blocks = get_equivalence_blocks preorder in
	let repre_states,represented_by = get_representants blocks aut.a_states in
	let repre_rules = merge_rules aut.a_rules repre_states represented_by in
	{a_rules=repre_rules;a_states=repre_states;a_alphabet=aut.a_alphabet},blocks

(*	danger !!*)
let relation_on_blocks blocks preorder =
(*	let blocks = get_equivalence_blocks preorder in*)
	let bc = List.length blocks in
	let m = Array.make_matrix bc bc false in
	let ba = Array.of_list blocks in
	for i = 0 to bc-1 do
		for j = 0 to bc-1 do
			m.(i).(j) <- preorder.(List.hd ba.(i)).(List.hd ba.(j))
		done
	done;
	m

			(*

	let ba = Array.of_list blocks in
	Array.map (fun bq ->  
		Array.map (fun br ->
			preorder.(List.hd br).(List.hd bq)
		) ba
	) ba
	*)

