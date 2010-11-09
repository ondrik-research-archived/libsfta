
	%{
		exception Parse_error of string;;

		let parse_error arg =
		  raise (Parse_error arg)
		;;

		let lookup tbl ind msg =
		  try Hashtbl.find tbl ind with Not_found -> raise (Parse_error msg)
		;;

		open Interim;;

		let halphabet = Hashtbl.create 53;;
		let alphabet = ref (Array.make 0 { f_index=0; f_name=""; f_rank=0 });;

		let hstates = Hashtbl.create 53;;
		let build_states names final =
		  Hashtbl.clear hstates;
		  let i = ref (-1) in
		    Array.of_list (
		      List.map (
		        fun x ->
		          incr(i);
		          let state = { s_index = !i; s_name = x; s_final = Hashtbl.mem final x } in
		            let _ = Hashtbl.add hstates x state in
		              state
		      )
		      names
		    )
		;;

	%}

        /* File parser.mly */
	%token EOF
        %token <int> NUM
	%token <string> ID
	%token OPS AUT STAT FIN TRANS
        %token LA COL COM LPAR RPAR
        %start main             /* the entry point */
	%type <(string * automaton) list> main

        %%

        main
	 :	alph aut_list EOF			{ List.rev $2 }
         ;

	alph
	 :	OPS sym_list				{ alphabet := Array.of_list (List.rev $2) }
	 ;

	sym_list
	 :	sym_list sym				{ let symbol = { f_index = List.length $1; f_name = fst $2; f_rank = snd $2 } in
							    let _ = Hashtbl.add halphabet (fst $2) symbol in
							      symbol::$1 }
	 |						{ [] }
	 ;

	sym
	 :	ID COL NUM				{ ($1, $3) }
	 ;

	aut_list
	 :	aut_list aut				{ $2::$1 }
	 |						{ [] }
	 ;

	aut
	 :	head decl transitions			{ ($1, { a_alphabet = !alphabet; a_states = $2; a_rules = $3 }) }	
	 ;

	head	
	 :	AUT ID					{ $2 }
	 ;

	decl
	 :	STAT decl_list FIN STAT final_list	{ build_states (List.rev $2) $5 }
	 ;

	decl_list
	 :	decl_list state_decl			{ $2::$1 }
	 |						{ [] }
	 ;

	state_decl
	 :	ID					{ $1 }
	 |	ID COL NUM				{ $1 }
	 ;

	final_list
	 :	final_list final_state			{ Hashtbl.add $1 $2 None; $1 }
	 |						{ Hashtbl.create 53 }
	 ;

	final_state
	 :	ID					{ $1 }
	 ;

	transitions
	 :	TRANS trans_list			{ List.rev $2 }
	 ;

	trans_list
	 :	trans_list trans			{ $2::$1 }
	 |						{ [] }
	 ;

	trans
	 :	ID state_list_par LA state		{ let symbol = lookup halphabet $1 ("undeclared symbol - "^$1) in (
							    if symbol.f_rank != List.length $2 then
							      parse_error ("symbol rank mismatch - "^$1);
							    { r_symbol = symbol.f_index; r_lhs = Array.of_list $2; r_rhs = $4 }
							  ) }
	 ;

	state_list_par
	 :	LPAR state_list RPAR			{ List.rev $2 }
	 |	LPAR RPAR				{ [] }
	 |					{ [] }
	 ;

	state_list
	 :	state_list COM state			{ $3::$1 }
	 |	state					{ [ $1 ] }
	 ;

	state
	 :	ID					{ (lookup hstates $1 ("undeclared state - "^$1)).s_index }
	 ;
