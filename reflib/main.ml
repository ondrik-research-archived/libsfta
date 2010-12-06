let print_rel = 
	Array.iteri (fun i row ->
		Array.iteri (fun j inrel ->
			if inrel then Printf.printf "(S%d, S%d),\n " i j
		) row
	)

open Lexing;;	
open Interim;;
(*open TreeSimLTS;;*)
(*open HHK;;*)

let get_mem (x : unit) = (Gc.stat ()).Gc.top_heap_words * (Sys.word_size / 8);;
	
let print_mem (x: unit) =
(*		Printf.printf "memory: %f\n" ((float_of_int Nativeint.size) *. (s.Gc.minor_words +. s.Gc.major_words -. s.Gc.promoted_words) /. (8. *. 1024. *. 1024.));;*)
		Printf.printf "memory: %d KiB\n" ((get_mem ()) / (8 * 1024));;

let _time_stack = ref [];;
let _time = ref 0.;;
let _mem = ref 0;;
let reset (x : unit) = (
	_mem := get_mem ();
	_time := Sys.time ()
);;

let eval (x : unit) = (
(* 	Printf.eprintf "time: %.3f s, space %d KiB\n" (Sys.time () -. !_time) ((get_mem () - !_mem) / 1024);*)
	flush_all ()
);;

let push_time (x : unit) = (
	_time_stack := !_time::!_time_stack;
	_time := Sys.time ();
);;

let pop_time (x : unit) = (
	_time := List.hd !_time_stack;
	_time_stack := List.tl !_time_stack;
);;

let print_bool x = if x then Printf.printf "1" else Printf.printf "0"
let print_bool_array = Array.iter print_bool
let print_bool_matrix = Array.iter (fun a -> print_bool_array a; Printf.printf "\n")

let to_taml_str n a =
	let str = Interim.to_string a
	and alph = Interim.alphabet_to_string a.a_alphabet in
	  "Ops "^alph^"\nAutomaton "^n^"\n"^str
;;

let auts_from_file name =
(*	let lexbuf = Lexing.from_channel (open_in Sys.argv.(3)) in*)
	let lexbuf = Lexing.from_channel (open_in name) in
	  try
      List.filter (fun a -> Array.length a.a_states > 0) (List.map snd (Parser.main Scanner.token lexbuf))
    with Parser.Parse_error e -> failwith (Printf.sprintf "error at line %s: %s\n" (string_of_int lexbuf.lex_curr_p.pos_lnum) e)
;;

let ltss_from_lexbuf lexbuf =
	  try
      List.map (fun a ->
				let lts = new Lts.lts (Array.length a.a_alphabet) (Array.length a.a_states) in (
					List.iter (fun r ->
						match Array.length r.r_lhs with
							| 0 -> ()
							| 1 -> lts#newTransition r.r_lhs.(0) r.r_symbol r.r_rhs
							| _ -> failwith "not an lts"						
					) a.a_rules;
					lts
				)
			) (List.filter (fun a -> Array.length a.a_states > 0) (List.map snd (Parser.main Scanner.token lexbuf)))
    with Parser.Parse_error e -> failwith (Printf.sprintf "error at line %s: %s\n" (string_of_int lexbuf.lex_curr_p.pos_lnum) e)
;;

let ltss_from_file name =
	let lexbuf = Lexing.from_channel (open_in name) in
	ltss_from_lexbuf lexbuf

let ltss_from_string str =
	let lexbuf = Lexing.from_string str in
	ltss_from_lexbuf lexbuf

let random_lts (x: unit) =
	Lts.generate_lts (int_of_string Sys.argv.(3)) (int_of_string Sys.argv.(4)) (float_of_string Sys.argv.(5))
;;

let random_aut (x: unit) =
	Interim.generate_a (int_of_string Sys.argv.(3)) (int_of_string Sys.argv.(4)) (int_of_string Sys.argv.(5)) (float_of_string Sys.argv.(6))
;;

(*
let create_pr lts =
	match Sys.argv.(1) with
		| "v0" -> ((new Rantap.prPair lts) :> RantapBase.prPairBase)
(*		| "v1" ->	((new Rantap2.prPair lts) :> RantapBase.prPairBase)*)
		| "v2" -> ((new Rantap3.prPair lts) :> RantapBase.prPairBase)
		| _ -> 	failwith "unknown variant"
*)

let create_pr lts = ((new Rantap3.prPair lts) :> RantapBase.prPairBase)

let compute_lts lts =
(*	Printf.printf "lts: %s\n" lts#stats;*)
(*	Printf.printf "%s " lts#stats;*)
(*	flush_all ();*)
  let pr = create_pr lts in (
	  pr#init;
    pr#compute;
    pr
	)
;;

let downward a =
	let lts = Translation2.downward a in
    compute_lts lts
;;

let upward a dwn_sim = (
	push_time ();
	let (lts, (env_partition, relation)) = Translation2.upward a dwn_sim in (
		eval ();
		pop_time ();
(*  	Printf.printf "lts: %s\n" lts#stats;*)
	  flush_all ();
		let pr = create_pr lts in (
			(* let non-accepting states be in block n. 0 *)
			(* split final states as block n. 1 *)
			pr#fakeSplit (List.map (fun x -> x.s_index) (List.filter (fun x -> x.s_final) (Array.to_list a.a_states)));
  		(* split environments as blocks n. 2, 3, 4, ... *)
			List.iter (fun x -> pr#fakeSplit x) env_partition;
			pr#get_rel#load relation;
			pr#init;
			pr#compute;
			pr
		)
	)
)
;;

let process_auts al do_up = (
  reset ();
	List.iter (fun a ->
		Printf.printf "ta: states: %d, symbols: %d, rank: %d, transitions: %d\n" (Array.length a.a_states) (Array.length a.a_alphabet) (Interim.rank a) (List.length a.a_rules);
		let r = ref (Array.make_matrix 0 0 false) in (
      let pr = downward a in
				r := pr#get_sim (Array.length a.a_states);
		  if do_up then (
			  let pr = upward a !r in
				  r := pr#get_sim (Array.length a.a_states)
			);
(*      print_bool_matrix !r*)
(*	print_rel !r;*)
		)
	) al;
  eval ()
)
;;

let process_lts lts = (
	reset ();
	let pr = compute_lts lts in (
(*    eval ();*)
    print_bool_matrix (pr#get_sim lts#nstates)
(*    print_rel (pr#get_sim lts#nstates)*)
	)
);;

let process_ltss ltsl = (
	reset ();
	List.iter (fun lts -> print_bool_matrix ((compute_lts lts)#get_sim lts#nstates)) ltsl;
  eval ()
);;

(*			  List.iter (fun (n, a) ->
					let oc = open_out ("aut/" ^ n ^ ".spec") in (
						Printf.fprintf oc "%s" (to_taml_str n a);
						close_out oc
					)
				) result;*)

let _ =
	if Sys.file_exists "random.init" then
		let ic = open_in "random.init" in (
			Random.init (int_of_string (input_line ic));
			close_in ic
		)
  else
		Random.self_init ()
;;

(*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*)

module UI_eq = Algorithms.UnIn(Incl.Equality.StateSet)(Incl.Equality.StateStateSetF(Incl.Equality.StateSet))
module UI_incl = Algorithms.UnIn(Incl.Inclusion.StateSet)(Incl.Inclusion.StateStateSetF(Incl.Inclusion.StateSet))
module UI_sim = Algorithms.UnIn(Incl.Simulation.StateSet)(Incl.Simulation.StateStateSetF(Incl.Simulation.StateSet))

(*
let compose_matrix m1 m2 =
	let l1,l2 = Array.length m1,Array.length m2 in
	Array.init (l1+l2) (fun i -> 
		if i<l1 then Array.append m1.(i) (Array.make l2 false)
		else Array.append (Array.make l1 false) m2.(i-l1)
	) 
*)

(*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*)
(*%%%%%% variants of word automata inclusion %%%%%%%%%%%%*)
(*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*)

let compose_matrix m1 m2 =
	let l1,l2 = Array.length m1,Array.length m2 in
	let m = Array.make_matrix (l1+l2) (l1+l2) false in
	for i=0 to l1-1 do
		for j=0 to l1-1 do
			m.(i).(j)<-m1.(i).(j)
		done
	done;
	for i=0 to l2-1 do
		for j=0 to l2-1 do
			m.(i+l1).(j+l1)<-m2.(i).(j)
		done
	done;
	m
			
let aut_stats a =
(*	Printf.sprintf "States:%d,Symbols:%d,Rules:%d" (Interim.sn a) (Interim.an a) (Interim.rn a)*)
	Printf.sprintf "%d" (Interim.sn a)  
 
let time (x : unit) = int_of_float (1000. *. (Sys.time () -. !_time));;

let wincl_unionsim a b =
	print_string"FORWARD_FULL,";flush_all();
	let fa,fb = Interim.final_states_into_one a, Interim.final_states_into_one b in 
	let ab = Interim.union fa fb in
	reset ();
	let lts = List.hd (ltss_from_string (to_taml_str "A" ab)) in
	let pr = compute_lts lts in
	Incl.Simulation.simulation := pr#get_sim lts#nstates;
	Printf.printf "CompSim,%d," (time ());
	push_time ();
	let result = (UI_sim.is_language_included fa fb) in
	Printf.printf "CompIncl,%d," (time ());
	pop_time ();
	Printf.printf "Time,%d," (time ());
	Printf.printf "Result,%B," result;
	result

let wincl_separatesim a b =
	print_string"FORWARD_SEPARATE,";flush_all();
	let fa,fb = Interim.final_states_into_one a, Interim.final_states_into_one b in 
	reset ();
	let ltsa= List.hd (ltss_from_string (to_taml_str "A" fa)) in
	let ltsb= List.hd (ltss_from_string (to_taml_str "B" fb)) in
	let pra = compute_lts ltsa in
	let prb = compute_lts ltsb in
	Printf.printf "CompSim,%d," (time ());
	push_time ();
	Incl.Simulation.simulation := compose_matrix (pra#get_sim ltsa#nstates) (prb#get_sim ltsb#nstates);
	Printf.printf "ComposeMatrix,%d," (time ());
	pop_time();
	push_time ();
	let result = (UI_sim.is_language_included fa fb) in
	Printf.printf "CompIncl,%d," (time ());
	pop_time ();
	Printf.printf "Time,%d," (time ());
	Printf.printf "Result,%B," result;
	result

let wincl_localsim a b =
	print_string"LOCAL,";flush_all();
	let fa,fb = Interim.final_states_into_one a, Interim.final_states_into_one b in 
	reset ();
	let sima = Localsim.simulation fa in
	let simb = Localsim.simulation fb in
	Incl.Simulation.simulation := compose_matrix sima simb;
	Printf.printf "CompSim,%d," (time ());
	push_time ();
	let result = (UI_sim.is_language_included fa fb) in
	Printf.printf "CompIncl,%d," (time ());
	pop_time ();
	Printf.printf "Time,%d," (time ());
	Printf.printf "Result,%B," result;
	result

let wincl_inclusion a b = 
	print_string"ALASKA,";flush_all();
	reset ();
	let result = UI_incl.is_language_included a b in
	Printf.printf "Time,%d," (time ());
	Printf.printf "Result,%B," result;
	result

let wincl_equality a b = 
	print_string"CLASSICAL,";flush_all();
	reset ();
	let result = UI_eq.is_language_included a b in
	Printf.printf "Time,%d," (time ());
	Printf.printf "Result,%B," result;
	result

let get_sub rel n = 
	let l = Array.length rel in
	Array.map (fun a -> Array.sub a 0 n) (Array.sub rel 0 n),
	Array.map (fun a -> Array.sub a n (l-n)) (Array.sub rel n (l-n))

let process_word_aut a =
	let fa = Interim.final_states_into_one a in
	let lts = List.hd (ltss_from_string (to_taml_str "A" fa)) in
(*	process_lts lts;*)
	let pr = compute_lts lts in
	let rel,_ = get_sub  (pr#get_sim lts#nstates) (Interim.sn a) in
(*    eval ();*)
    print_bool_matrix rel


(*	assume that final_states_into_one has been called*)
let sim a = 
	let lts = List.hd (ltss_from_string (to_taml_str "A" a)) in
	let pr = compute_lts lts in
	pr#get_sim lts#nstates

let wreduce a =  
	fst (Merge.merge a (sim a))



let shift_blocks n bl  =
	let shift_block n b = List.map (fun q -> q+n) b in
	List.map (shift_block n) bl

let wincl_reduced_unionsim a b =
	print_string"REDUCED_FORWARD_FULL,";flush_all();
	let fa,fb = Interim.final_states_into_one a, Interim.final_states_into_one b in 
	let ab = Interim.union fa fb in
	reset ();
	let lts = List.hd (ltss_from_string (to_taml_str "A" ab)) in
	let pr = compute_lts lts in
	let simulation = pr#get_sim lts#nstates in
	Printf.printf "CompSim,%d," (time ());
	push_time ();

	let rela,relb = get_sub simulation (Interim.sn fa) in
	let (rfa,blocksa),(rfb,blocksb) = Merge.merge fa rela,Merge.merge fb relb in

(*	let ra,rb = (sim fa),(sim fb) in *)
(*	if ra<>rela || rb<>relb then failwith"relations differs2";*)

	Printf.printf "ReducedSize1,%s,ReducedSize2,%s,"(aut_stats rfa) (aut_stats rfb); flush_all ();
	let shift = Interim.sn fa in
	Incl.Simulation.simulation := Merge.relation_on_blocks (blocksa@(shift_blocks shift blocksb)) simulation;

(*	let check = sim (Interim.union rfa rfb) in*)
(*	if not (Rel.contains  check(!Incl.Simulation.simulation)) then failwith"relations differs3";*)
(*	if check<> (!Incl.Simulation.simulation) then failwith"relations differs4";*)

	Printf.printf "Merge,%d," (time ());
	push_time ();
	let result = (UI_sim.is_language_included rfa rfb) in
	Printf.printf "CompIncl,%d," (time ());
	pop_time ();
	pop_time ();
	Printf.printf "Time,%d," (time ());
	Printf.printf "Result,%B," result;
	result

let wincl_reduced_separatesim a b =
	print_string"REDUCED_FORWARD_SEPARATE,";flush_all();
	let fa,fb = Interim.final_states_into_one a, Interim.final_states_into_one b in 
	reset ();
	let ltsa= List.hd (ltss_from_string (to_taml_str "A" fa)) in
	let ltsb= List.hd (ltss_from_string (to_taml_str "B" fb)) in
	let pra = compute_lts ltsa in
	let prb = compute_lts ltsb in
	Printf.printf "CompSim,%d," (time ());
	push_time ();
	let rela,relb = (pra#get_sim ltsa#nstates), (prb#get_sim ltsb#nstates) in
	let (rfa,blocksa),(rfb,blocksb) = Merge.merge fa rela,Merge.merge fb relb in
	let rel2a,rel2b =  Merge.relation_on_blocks blocksa rela, Merge.relation_on_blocks blocksb relb in
	push_time ();
	Incl.Simulation.simulation := compose_matrix rel2a rel2b;
	Printf.printf "ComposeMatrix,%d," (time ());
	pop_time();
	Printf.printf "Merge,%d," (time ());
	pop_time();
	push_time();
	let result = (UI_sim.is_language_included rfa rfb) in
	Printf.printf "CompIncl,%d," (time ());
	pop_time ();
	Printf.printf "Time,%d," (time ());
	Printf.printf "Result,%B," result;
	result

let wincl_reduced_inclusion a b = 
	print_string"REDUCED_ALASKA,";flush_all();
	let fa,fb = Interim.final_states_into_one a, Interim.final_states_into_one b in 
	reset ();
	let rfa,rfb = wreduce fa,wreduce fb in
	Printf.printf "Reducing,%d," (time ());
	Printf.printf "ReducedSize1,%s,ReducedSize2,%s,"(aut_stats rfa) (aut_stats rfb); flush_all ();
	push_time();
	let result = UI_incl.is_language_included rfa rfb in
	Printf.printf "CompIncl,%d," (time ());
	pop_time();
	Printf.printf "Time,%d," (time ());
	Printf.printf "Result,%B," result;
	result


(*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*)
(*%%%%%% variants of word automata univerality %%%%%%%%%%*)
(*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*)

let wuni_equality a = 
	print_string"CLASSICAL,";flush_all();
	reset ();
	let result = UI_eq.is_universal a in
	Printf.printf "Time,%d," (time ());
	Printf.printf "Result,%B," result;
	result

let wuni_inclusion a = 
	print_string"ALASKA,";flush_all();
	reset ();
	let result = UI_incl.is_universal a in
	Printf.printf "Time,%d," (time ());
	Printf.printf "Result,%B," result;
	result

let wuni_reduced_inclusion a = 
	print_string"REDUCED_ALASKA,";flush_all();
	let fa = Interim.final_states_into_one a in
	reset ();
	let lts = List.hd (ltss_from_string (to_taml_str "A" fa)) in
	let pr = compute_lts lts in
	let rel,_ = get_sub  (pr#get_sim lts#nstates) (Interim.sn a) in
	let ra,_ = Merge.merge a rel in
	Printf.printf "Reducing,%d," (time ());flush_all();
	push_time ();
	let result = UI_incl.is_universal ra in
	Printf.printf "CompUniv,%d," (time ());
	pop_time ();
	Printf.printf "Time,%d," (time ());
	Printf.printf "Result,%B," result;
	result

let wuni_simulation a =
	print_string"FORWARD,";flush_all();
	let fa = Interim.final_states_into_one a in 
	reset ();
	let lts = List.hd (ltss_from_string (to_taml_str "A" fa)) in
	let pr = compute_lts lts in
	let rel,_ = get_sub  (pr#get_sim lts#nstates) (Interim.sn a) in
(*	Interim.print fa;*)
(*	print_string("rel1\n");*)
(*print_rel (pr#get_sim lts#nstates);*)
(*	let _,rel = get_sub  (pr#get_sim lts#nstates) 1 in*)
(*	print_string("rel2\n");*)
(*print_rel rel;*)
(*if Rel.is_reflexive rel then print_string "refl" else print_string "irelfl";*)
	Incl.Simulation.simulation := rel;
(*	Incl.Simulation.simulation := Rel.identity (Interim.sn a);*)
	Printf.printf "CompSim,%d," (time ());
	push_time ();
	let result = (UI_sim.is_universal a) in
	Printf.printf "CompUniv,%d," (time ());
	pop_time ();
	Printf.printf "Time,%d," (time ());
	Printf.printf "Result,%B," result;
	result

let wuni_local a =
	print_string"LOCAL,";flush_all();
	let fa = Interim.final_states_into_one a in 
	reset ();
	let rel,_ = get_sub  (Localsim.simulation fa) (Interim.sn a) in
	Incl.Simulation.simulation := rel;
	Printf.printf "CompSim,%d," (time ());
	push_time ();
	let result = (UI_sim.is_universal a) in
	Printf.printf "CompUniv,%d," (time ());
	pop_time ();
	Printf.printf "Time,%d," (time ());
	Printf.printf "Result,%B," result;
	result

let wuni_reduced_simulation a =
	print_string"REDUCED_FORWARD,";flush_all();
	let fa = Interim.final_states_into_one a in 
	reset ();
	let lts = List.hd (ltss_from_string (to_taml_str "A" fa)) in
	let pr = compute_lts lts in
	Printf.printf "CompSim,%d," (time ());flush_all();
	push_time ();
	let rel,_ = get_sub  (pr#get_sim lts#nstates) (Interim.sn a) in
	let ra,blocks = Merge.merge a rel in
	Incl.Simulation.simulation := Merge.relation_on_blocks blocks rel;
	Printf.printf "Merge,%d," (time ());flush_all();
	pop_time ();
	push_time ();
	let result = UI_sim.is_universal ra in
	Printf.printf "CompIncl,%d," (time ());flush_all();
	pop_time ();
	Printf.printf "Time,%d," (time ());flush_all();
	Printf.printf "Result,%B," result;flush_all();
	result

let rec allthesame = function
	[] -> true
	| [_] -> true 
	| a::b::tl -> (a=b) && (allthesame (b::tl)) 

(*let inclusion ?(pattern="armcNFA_inclTest_") which_inclusion  dir n =*)
let inclusion which_inclusion dir pattern n =
	let a = Interim.reverse_word_automaton (List.hd (auts_from_file (dir^"/"^pattern^n))) in
	let b = Interim.reverse_word_automaton (List.hd (auts_from_file (dir^"/"^pattern^(string_of_int ((int_of_string n)+1))))) in

	Printf.printf "Backward,WhichIncl,%s,Expr,%s_%s,Size1,%s,Size2,%s,"which_inclusion dir n (aut_stats a) (aut_stats b); flush_all ();

	let a,b = 
		match which_inclusion with 
		"ab" -> a,b
		| "ba" -> b,a  
		| _ -> failwith "don't know what to do"
	in
	if not (allthesame [
(*		wincl_equality a b;*)
		wincl_inclusion a b;
(*		wincl_separatesim a b;*)
		wincl_unionsim a b;
(*		wincl_localsim a b;*)
(*		wincl_reduced_unionsim a b;*)
(*		wincl_reduced_separatesim a b;*)
(*		wincl_reduced_inclusion a b;*)
	] ) then failwith"different results!"
	;
	print_newline()

let universality dir pattern n =
	let a = List.hd (auts_from_file (dir^"/"^pattern^n)) in

	Printf.printf "Expr,%s_%s,Size,%s," dir n (aut_stats a); flush_all ();

	if not (allthesame [
		wuni_equality a;
		wuni_inclusion a;
		wuni_simulation a;
		wuni_local a;
		wuni_reduced_inclusion a;
		wuni_reduced_simulation a;
	] ) then failwith"different results!"
	;
	print_newline()

(*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*)
(*%%%%%% tree automata universality            %%%%%%%%%%*)
(*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*)

let tuni_inclusion a = 
	reset ();
	print_string"INCLUSION,";flush_all();
	let result = UI_incl.is_universal a in
	Printf.printf "Time,%d," (time ());
	Printf.printf "Result,%B," result;flush_all()

let tuni_simulation a = 
	print_string"UPWARD,";flush_all();
	let fa = Interim.final_states_into_one a in 
	reset();
	let id = Rel.identity (Interim.sn fa) in
	let pr = upward fa id in
	let upward = pr#get_sim (Interim.sn fa) in
	let rel,_ = get_sub upward (Interim.sn a) in
	Incl.Simulation.simulation := rel;
	Printf.printf "Rel,%d," (Rel.cardinality rel);
	push_time ();
	Printf.printf "CompUniv,%d," (time ());flush_all();
	pop_time ();
	let result = UI_sim.is_universal a in
	Printf.printf "Time,%d," (time ());
	Printf.printf "Result,%B," result;flush_all()

let tree_universality file =
	let al = auts_from_file file in 
	List.iter (fun a ->
		Printf.printf "File,%s,Size,%s," file (aut_stats a);
		if not (allthesame [
(*			tuni_equality a;*)
			tuni_inclusion a;
			tuni_simulation a;
(*			tuni_local a;*)
(*			tuni_reduced_inclusion a;*)
(*			tuni_reduced_simulation a;*)
		] ) then failwith"different results!"
		;
		print_newline()
	) al

(*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*)
(*%%%%%% tree automata inclusion               %%%%%%%%%%*)
(*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*)

let treduce_down a =
	push_time();
	let pr = downward a in
	let rel = pr#get_sim (Interim.sn a) in
	let res = fst (Merge.merge a rel) in
	Printf.printf "ReduceDown,%d," (time ());
	pop_time();
	res

let treduce_up a =
	let fa = Interim.final_states_into_one a in 
	push_time();
	let id = Rel.identity (Interim.sn fa) in
	let pr = upward fa id in
	let upsim = pr#get_sim (Interim.sn fa) in
	let rel,_ = get_sub upsim (Interim.sn a) in
	let res = fst (Merge.merge a rel) in
	Printf.printf "ReduceUp,%d," (time ());
	pop_time();
	res

let treduce_med a =
	let fa = Interim.final_states_into_one a in 
	push_time();
	let pr = downward a in
	let downsim = pr#get_sim (Interim.sn a) in
	let id = Rel.identity (Interim.sn fa) in
	let pr = upward fa id in
	let upward = pr#get_sim (Interim.sn fa) in
	let upsim,_ = get_sub upward (Interim.sn a) in
	let usefull = Rel.usefull downsim upsim in
	let res = fst (Merge.merge a usefull) in
	Printf.printf "ReduceComp,%d," (time ());
	pop_time();
	res

let tincl_inclusion a b = 
	let fa,fb = Interim.final_states_into_one a, Interim.final_states_into_one b in 
	print_string"INCLUSION,";flush_all();
	reset ();
	let result = UI_incl.is_language_included fa fb in
	Printf.printf "Time,%d," (time ());
	Printf.printf "Result,%B," result;
	result

let tincl_unionsim a b =
	print_string"UPWARD_FULL,";flush_all();
	let fa,fb = Interim.final_states_into_one a, Interim.final_states_into_one b in 
	reset ();
	let ab = Interim.union fa fb in
	let id = Rel.identity (Interim.sn ab) in
	let pr = upward ab id in
	let rel = pr#get_sim (Interim.sn ab) in
	Incl.Simulation.simulation := rel;
	Printf.printf "CompSim,%d," (time ());
	Printf.printf "Rel,%d," (Rel.cardinality rel);
	push_time ();
	let result = (UI_sim.is_language_included fa fb) in
	Printf.printf "CompIncl,%d," (time ());
	pop_time ();
	Printf.printf "Time,%d," (time ());
	Printf.printf "Result,%B," result;
	result

let tincl_separatesim a b =
	print_string"UPWARD_SEPARATE,";flush_all();
	let fa,fb = Interim.final_states_into_one a, Interim.final_states_into_one b in 
	reset ();
	let ida,idb = Rel.identity (Interim.sn fa),Rel.identity (Interim.sn fb) in
	let pra,prb = upward fa ida, upward fb idb in
	let rela,relb = pra#get_sim (Interim.sn fa),prb#get_sim (Interim.sn fb) in
	Printf.printf "CompSim,%d," (time ());
	push_time ();
	Incl.Simulation.simulation := compose_matrix rela relb;
	Printf.printf "ComposeMatrix,%d," (time ());
	pop_time();
	push_time ();
	let result = (UI_sim.is_language_included fa fb) in
	Printf.printf "CompIncl,%d," (time ());
	pop_time ();
	Printf.printf "Time,%d," (time ());
	Printf.printf "Result,%B," result;
	result

let rec skip l n =
	if n < 1 then l else
	match l with
	[] -> []
	| h::t -> skip t (n-1)

let rec combine l1 l2 =
	match l1,l2 with
	[],_ -> []
	| _,[] -> []
	| h1::t1,h2::t2 -> (h1,h2)::(combine t1 t2)

let tree_inclusion reduction filea fileb nskip =
	let nskip = int_of_string nskip in
	let sort = List.sort (fun (a,b) (c,d) -> compare (Interim.sn b) (Interim.sn d)) in
	print_string "readig files ...";flush_all ();
	let al,bl = auts_from_file filea, auts_from_file fileb in 
	let l = skip (sort (combine al bl)) nskip in
	print_string " done\n";flush_all ();
	Printf.printf "file %s: %d automata\n" filea (List.length al);
	Printf.printf "file %s: %d automata\n" fileb (List.length bl);
	List.iter (fun (a,b) ->
		Printf.printf "FileA,%s,FileB,%s,SizeA,%s,SizeB,%s," filea fileb (aut_stats a) (aut_stats b);
		let a,b = match reduction with 
			"no" -> a,b 
			| "down" -> treduce_down a,treduce_down b
			| "up" -> treduce_up a,treduce_up b 
			| "med" -> treduce_med a,treduce_med b 
			| _ -> failwith"wrong reduction type"
	   in
		Printf.printf "RedSizeA,%s,RedSizeB,%s," (aut_stats a) (aut_stats b);
		if not (allthesame [
(*			tincl_equality a b;*)
			tincl_inclusion a b;
			tincl_separatesim a b;
			tincl_unionsim a b;
(*			tincl_localsim a b;*)
(*			tincl_reduced_unionsim a b;*)
(*			tincl_reduced_separatesim a b;*)
(*			tincl_reduced_inclusion a b;*)
		] ) then failwith"different results!"
		;
		print_newline()
	) l

module Ordered_string =
    struct
       type t = string
       let compare s1 s2 = compare s1 s2
    end

module String_map = Map.Make(Ordered_string)

let union_of_alphabets alph1 alph2 =
         let symbols1 = Array.fold_left (fun map a -> String_map.add
a.f_name a map) String_map.empty alph1  in
         let symbols12 = Array.fold_left (fun map a ->
                 try (
                         let a_alias = String_map.find a.f_name map in
                         if a_alias.f_rank <> a.f_rank then

failwith"Interim.union_of_alphabets:automata contain a symbol
                         with the same name but different ranks";
                         map
                 ) with Not_found ->
                         String_map.add a.f_name a map
                 )
         symbols1 alph2
         in


         let symarray = Array.of_list (List.rev (((String_map.fold (fun
_ a l -> a::l))) symbols12 [])) in
         Array.mapi (fun i {f_name = name;f_rank = rank} ->
                 {f_name = name;f_rank = rank;f_index = i}) symarray

(*changes the alphabet of an automaton into alph. alph must contain the
  * original alphabet*)
let change_alphabet aut alph =
         let symbols = Array.fold_left (fun map a ->
                 String_map.add a.f_name a.f_index map) String_map.empty
alph
         in
         let new_indices = Array.map (fun _ -> 0) aut.a_alphabet in
         Array.iter (fun {f_name=name;f_index = old_i} ->
                 let new_i =
                 try String_map.find name symbols
                 with Not_found -> (failwith"Interm.change_alphabet:the new alphabet
                 does not contain the original one")
                 in
                 new_indices.(old_i) <- new_i;
         ) aut.a_alphabet;
         let new_rules =  List.map (fun
{r_symbol=old_i;r_lhs=rhs;r_rhs=lhs} ->
                 {r_symbol=new_indices.(old_i);r_lhs=rhs;r_rhs=lhs}
         ) aut.a_rules in
         {
                 a_alphabet=alph;
                 a_states=aut.a_states;
                 a_rules=new_rules;
         }

let unify_alphabets (aut1,aut2) =
         let alph = union_of_alphabets aut1.a_alphabet aut2.a_alphabet in
         (change_alphabet aut1 alph),(change_alphabet aut2 alph)
let dopocitej_ondrovi_aritu aut =
  List.iter (fun {r_symbol = sym;r_lhs = lhs} ->
    match aut.a_alphabet.(sym) with {f_name = name} ->
      aut.a_alphabet.(sym) <- {f_index = sym;f_name =
        name;f_rank = Array.length lhs})
  aut.a_rules; aut


let load_two_automata file1 file2 =
  let l1 = (List.hd (auts_from_file file1)) in
  let l2 = (List.hd (auts_from_file file2)) in
  let l1_with_arity = (dopocitej_ondrovi_aritu l1) in
  let l2_with_arity = (dopocitej_ondrovi_aritu l2) in
    unify_alphabets (l1_with_arity, l2_with_arity)

let tree_equivalence file1 file2 =
  let (l1, l2) = (load_two_automata file1 file2) in
	let result = (UI_incl.is_language_included l1 l2) && (UI_incl.is_language_included l1 l2) in
	Printf.printf "%B\n" result;
;;

(*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*)

let _ =
	match Sys.argv.(1) with
		| "aut_fd" -> process_auts (auts_from_file Sys.argv.(2)) false
		| "aut_fu" -> process_auts (auts_from_file Sys.argv.(2)) true
		| "aut_d" -> process_auts [random_aut ()] false
		| "aut_u" -> process_auts [random_aut ()] true
		| "lts_f" -> process_ltss (ltss_from_file Sys.argv.(2))
		| "lts" ->  process_lts (random_lts ())
		| "cnt" -> Printf.printf "%d\n" (List.length (auts_from_file Sys.argv.(2)))

		| "incl" -> inclusion Sys.argv.(2) Sys.argv.(3) Sys.argv.(4) Sys.argv.(5)
		| "univ" -> universality Sys.argv.(2) Sys.argv.(3) Sys.argv.(4) 
		| "w_aut_f" -> process_word_aut (List.hd (auts_from_file Sys.argv.(2)))
		| "w_aut_b" -> process_word_aut (Interim.reverse_word_automaton (List.hd (auts_from_file Sys.argv.(2))))
		| "tree_univ" -> tree_universality Sys.argv.(2)
		| "tree_incl" -> tree_inclusion Sys.argv.(2) Sys.argv.(3) Sys.argv.(4) Sys.argv.(5)
    | "eq" -> tree_equivalence Sys.argv.(2) Sys.argv.(3)

		| _ -> failwith "don't know what to do"
;;
