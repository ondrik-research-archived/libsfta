
open Lexing;;	
open Interim;;
(*open TreeSimLTS;;
open HHK;;*)
(*
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
 	Printf.eprintf "time: %.3f s, space %d KiB\n" (Sys.time () -. !_time) ((get_mem () - !_mem) / 1024);
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
*)
let print_bool_array = Array.iter (fun x -> if x then Printf.printf "1" else Printf.printf "0")
let print_bool_matrix = Array.iter (fun a -> print_bool_array a; Printf.printf "\n")

let find_state_name stateIndex (aut : automaton) =
  Array.fold_left (fun from_ar (st :state) -> if st.s_index = stateIndex then st.s_name else from_ar) "error" aut.a_states
;;

let rec print_pretty_row index row (aut : automaton) putComma =
  if (index < Array.length row) then (
      if ((Array.get row index) == true) then (
        if (putComma) then Printf.printf ";";
        Printf.printf " ";
        Printf.printf "%s" (find_state_name index aut);
        print_pretty_row (index+1) row aut true;
      )
      else (
        print_pretty_row (index+1) row aut putComma;
      )
    )
;;

let rec print_pretty_matrix index matrix (aut : automaton) =
  if (index < Array.length matrix) then (
      Printf.printf " %s -> [" (find_state_name index aut) ;
      print_pretty_row 0 (Array.get matrix index) aut false;
      Printf.printf "]";
      if (index+1 == Array.length matrix) then (
      )
      else (
        Printf.printf ",";
        print_pretty_matrix (index+1) matrix aut;
      )
    )
    else (
      Printf.printf ""
    )
;;

let print_pretty_simulation matrix aut =
  Printf.printf "{";
  print_pretty_matrix 0 matrix aut;
  Printf.printf "}\n"
;;

let to_taml_str n a =
	let str = Interim.to_string a
	and alph = Interim.alphabet_to_string a.a_alphabet in
	  "Ops "^alph^"\nAutomaton "^n^"\n"^str
;;

let to_file fname n a =
  let f = (open_out fname) in (
    output_string f (to_taml_str n a);
    close_out f
  )
;;
(*
let timbuk_split dir l =
  List.iter (fun (n, a) ->
    to_file (dir ^ "/" ^ n) n a
  ) l
;;
*)
let auts_from_file_raw name =
	let lexbuf = Lexing.from_channel (open_in name) in
	  try
      Parser.main Scanner.token lexbuf
    with Parser.Parse_error e -> failwith (Printf.sprintf "error at line %s: %s\n" (string_of_int lexbuf.lex_curr_p.pos_lnum) e)
;;

let auts_from_file name =
	let lexbuf = Lexing.from_channel (open_in name) in
	  try
      List.filter (fun a -> Array.length a.a_states > 0) (List.map snd (Parser.main Scanner.token lexbuf))
    with Parser.Parse_error e -> failwith (Printf.sprintf "error at line %s: %s\n" (string_of_int lexbuf.lex_curr_p.pos_lnum) e)
;;

let ltss_from_file name =
	let lexbuf = Lexing.from_channel (open_in name) in
	  try
      List.map (fun a ->
				let lts = new Lts.lts (Array.length a.a_alphabet) (Array.length a.a_states) in (
					List.iter (fun r ->
						match Array.length r.r_lhs with
							| 0 -> ()
							| 1 -> lts#newTransition r.r_lhs.(0) r.r_symbol r.r_rhs
							| _ -> failwith "not an lts"						
					) a.a_rules;
					List.iter (fun x ->
						lts#newFinalState x.s_index
					) (List.filter (fun x -> x.s_final) (Array.to_list a.a_states));
					lts
				)
			) (List.filter (fun a -> Array.length a.a_states > 0) (List.map snd (Parser.main Scanner.token lexbuf)))
    with Parser.Parse_error e -> failwith (Printf.sprintf "error at line %s: %s\n" (string_of_int lexbuf.lex_curr_p.pos_lnum) e)
;;
(*
let random_lts (x: unit) =
	Lts.generate_lts (int_of_string Sys.argv.(3)) (int_of_string Sys.argv.(4)) (float_of_string Sys.argv.(5))
;;

let random_aut (x: unit) =
	Interim.generate_a (int_of_string Sys.argv.(3)) (int_of_string Sys.argv.(4)) (int_of_string Sys.argv.(5)) (float_of_string Sys.argv.(6))
;;

let random_aut_full (x: unit) = (
	let a = random_aut () in (
		Array.iter (fun x -> x.f_name <- "l" ^ (string_of_int x.f_index)) a.a_alphabet;
		Array.iter (fun x -> x.s_name <- "s" ^ (string_of_int x.s_index)) a.a_states;
		a
	)
)
*)

let create_pr lts =
(*	match Sys.argv.(1) with
		| "v0" -> ((new Rantap.prPair lts) :> RantapBase.prPairBase)
(*		| "v1" ->	((new Rantap2.prPair lts) :> RantapBase.prPairBase)*)
		| "v2" -> ((new Rantap3.prPair lts) :> RantapBase.prPairBase)
		| _ -> 	failwith "unknown variant"*)
	((new Sa.prPair lts) :> SaBase.prPairBase)

let compute_lts lts =
(*	Printf.printf "lts: %s\n" lts#stats;
	flush_all ();*)
  let pr = create_pr lts in (
	if (List.length lts#finalStates) > 0 then (
	  pr#fakeSplit lts#finalStates;
          let rel = Array.make_matrix 2 2 false in (
		List.iter (fun (x, y) -> rel.(x).(y) <- true) [(0,0); (0,1); (1,1)];
	  	pr#get_rel#load rel
	  )
	);
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
(*	push_time ();*)
	let (lts, (env_partition, relation)) = Translation2.upward a dwn_sim in (
(*		eval ();
		pop_time ();
  	Printf.printf "lts: %s\n" lts#stats;
	  flush_all ();*)
		let pr = create_pr lts in (
			(* let non-accepting states be in block n. 0 *)
			(* split final states as block n. 1 *)
			pr#fakeSplit lts#finalStates;
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

let process_auts al do_up pretty_print = (
(*  reset (); *)
	List.iter (fun a ->
(*		Printf.printf "ta: states: %d, symbols: %d, rank: %d, transitions: %d\n" (Array.length a.a_states) (Array.length a.a_alphabet) (Interim.rank a) (List.length a.a_rules);*)
		let r = ref (Array.make_matrix 0 0 false) in (
      let pr = downward a in
				r := pr#get_sim (Array.length a.a_states);
		  if do_up then (
			  let pr = upward a !r in
				  r := pr#get_sim (Array.length a.a_states)
			);
      if pretty_print then (
        print_pretty_simulation !r a
      )
      else (
        print_bool_matrix !r
      )
		)
	) al;
(*  eval ()*)
)
;;

let process_lts lts = (
	(*reset ();*)
	let pr = compute_lts lts in (
    (*eval ();*)
    print_bool_matrix (pr#get_sim lts#nstates)
	)
);;

let process_ltss ltsl = (
(*	reset ();*)
	List.iter (fun lts -> print_bool_matrix ((compute_lts lts)#get_sim lts#nstates)) ltsl;
(*  eval ()*)
);;

(*			  List.iter (fun (n, a) ->
					let oc = open_out ("aut/" ^ n ^ ".spec") in (
						Printf.fprintf oc "%s" (to_taml_str n a);
						close_out oc
					)
				) result;*)
(*
let _ =
	if Sys.file_exists "random.init" then
		let ic = open_in "random.init" in (
			Random.init (int_of_string (input_line ic));
			close_in ic
		)
  else
		Random.self_init ()
;;
*)

let print_usage () =
	Printf.printf "usage: \n";
	Printf.printf "%s lts|ta-down|ta-up <file>\n" Sys.argv.(0);
	Printf.printf "  lts\t\tcomputes simulation over given LTS\n";
	Printf.printf "  ta-down\tcomputes downward simulation over given TA\n";
	Printf.printf "  ta-up\t\tcomputes upward simulation over given TA\n";
  Printf.printf "\n";
	Printf.printf "  ta-down-pretty\tcomputes downward simulation over given TA and prints it in a pretty form\n";
	Printf.printf "  ta-up-pretty\t\tcomputes upward simulation over given TA and prints it in a pretty form\n";
;;

let _ =
(*	Printf.printf "\narguments:%s,%s,%s"  Sys.argv.(1) Sys.argv.(2) Sys.argv.(3);*)
	if Array.length Sys.argv = 3 then (
		match Sys.argv.(1) with
			| "lts" -> process_ltss (ltss_from_file Sys.argv.(2))
			| "ta-down" -> process_auts (auts_from_file Sys.argv.(2)) false false
			| "ta-up" -> process_auts (auts_from_file Sys.argv.(2)) true false
			| "ta-down-pretty" -> process_auts (auts_from_file Sys.argv.(2)) false true
			| "ta-up-pretty" -> process_auts (auts_from_file Sys.argv.(2)) true true
			| _ -> (print_usage (); exit (-1))
	) else (
		print_usage (); exit (-1)
	)
;;
