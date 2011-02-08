
open Interim;;
open Lts;;
open Hashtbl;;
let hashlen t = 
	let l = ref 0 in
	Hashtbl.iter (fun _  _ -> incr l) t;
	!l;;

let downward (a: automaton) =
  let rank = Interim.rank a
  and asize = Array.length a.a_alphabet
  and lhstbl = Hashtbl.create 53 in
	  let add_lhs lhs =
	    if not (Hashtbl.mem lhstbl lhs) then
(*	      Hashtbl.add lhstbl lhs (Array.length a.a_states + Hashtbl.length lhstbl);*)
	      Hashtbl.add lhstbl lhs (Array.length a.a_states + hashlen lhstbl);
	  and find_lhs lhs = Hashtbl.find lhstbl lhs in
	    let _ = List.iter (fun x -> add_lhs x.r_lhs) a.a_rules in
(*        let _lts = new lts (asize + rank) (Array.length a.a_states + Hashtbl.length lhstbl) in*)
        let _lts = new lts (asize + rank) (Array.length a.a_states +  hashlen lhstbl) in
	        let _ = Hashtbl.iter (fun x y ->
						Array.iteri (fun i x -> _lts#newTransition y (asize + i) x) x
					) lhstbl
		      and _ = List.iter (fun x ->
						_lts#newTransition x.r_rhs x.r_symbol (find_lhs x.r_lhs)
					) a.a_rules
(*		      and _ = List.iter (fun x ->
						_lts#newFinalState x.s_index
					) (List.filter (fun x -> x.s_final) (Array.to_list a.a_states))*)
				  in
			      _lts
;;

let upward (a: automaton) (dwnsim: bool array array) =
  let asize = Array.length a.a_alphabet
	and dwneq =
		let a = Array.make_matrix (Array.length dwnsim) (Array.length dwnsim) false in (
			for i = 0 to (Array.length a) - 1 do
				for j = 0 to (Array.length a) - 1 do
					a.(i).(j) <- (dwnsim.(i).(j) && dwnsim.(j).(i))
				done
			done;
			a
		)		
	in
	  let env_eq (lhs1, sym1, rhs1) (lhs2, sym2, rhs2) =
  		let rec lhs_eq = ( 
	  		let state_eq x y = (if x == -1 then (y == -1) else (if y == -1 then false else dwneq.(x).(y))) in
		  	  function
			  		| 0 -> state_eq lhs1.(0) lhs2.(0)
				  	| i -> (state_eq lhs1.(i) lhs2.(i)) && (lhs_eq (i - 1))
  		) in
	  	  (sym1 == sym2) && lhs_eq ((Array.length lhs1) - 1) 
	  and env_sim (lhs1, sym1, rhs1) (lhs2, sym2, rhs2) =
  		let rec lhs_sim = ( 
	  		let state_sim x y = (if x == -1 then (y == -1) else (if y == -1 then false else dwnsim.(x).(y))) in
		  	  function
			  		| 0 -> state_sim lhs1.(0) lhs2.(0)
				  	| i -> (state_sim lhs1.(i) lhs2.(i)) && (lhs_sim (i - 1))
  		) in
	  	  (sym1 == sym2) && lhs_sim ((Array.length lhs1) - 1) 
    and envtbl = Hashtbl.create 53
	  and partition = ref [] in
	    let array_replace a i x =
		    let r = Array.copy a in
			    let _ = r.(i) <- x in r
  		and reg_env env s = (
	  		let rec search_env l env s = match l with
		  		| [] -> [(env, [s])]  
			  	| (x, y)::z -> if env_eq env x then (x, s::y)::z else (x, y)::(search_env z env s)
			  in
			    partition := search_env !partition env s
  		)
			and build_pr l =
				let a = Array.of_list l in
				  let rel = Array.make_matrix ((Array.length a) + 2) ((Array.length a) + 2) false in (
						(* setup the relation on original states *)
						List.iter (fun (x, y) -> rel.(x).(y) <- true) [(0,0); (0,1); (1,1)];
					  Array.iteri (fun i (x, _) ->
						  Array.iteri (fun j (y, _) ->
								if env_sim x y then
									rel.(i + 2).(j + 2) <- true;
							) a
						) a;
				    (List.map snd l, rel)
					)
			in
	      let build_env lhs sym rhs i = (array_replace lhs i (-1), sym, rhs)
	      and add_env env f =
				  if not (Hashtbl.mem envtbl env) then
(*    			  let s = Array.length a.a_states + Hashtbl.length envtbl in ( *)
    			  let s = Array.length a.a_states + hashlen envtbl in (
              Hashtbl.add envtbl env s;
						  f env s
					  )
  	    and find_env env = Hashtbl.find envtbl env in
	        let _ = List.iter (fun x ->
		  	    for i = 0 to (Array.length x.r_lhs) - 1 do
			        let env = build_env x.r_lhs x.r_symbol x.r_rhs i in
				  		  add_env env reg_env
			      done
		      ) a.a_rules in
(*            let _lts = new lts (asize + 1) (Array.length a.a_states + Hashtbl.length envtbl) in*)
            let _lts = new lts (asize + 1) (Array.length a.a_states + hashlen envtbl) in
		          let _ = List.iter (fun x ->
     			      for i = 0 to (Array.length x.r_lhs) - 1 do
								  let env = find_env (build_env x.r_lhs x.r_symbol x.r_rhs i) in (
			              _lts#newTransition x.r_lhs.(i) asize env;
			              _lts#newTransition env x.r_symbol x.r_rhs
								  )
			          done;							
						  ) a.a_rules
			  and _ = List.iter (fun x ->
						_lts#newFinalState x.s_index
					    ) (List.filter (fun x -> x.s_final) (Array.to_list a.a_states))
			 in
			          (_lts, build_pr !partition)
;;
