
(* File scanner.mll *)
{
	open Parser        (* The type token is defined in parser.mli *)

	let keywords = Hashtbl.create 53

	let _ = List.iter (fun (kwd, tok) -> Hashtbl.add keywords kwd tok) [
		"Ops", OPS;
		"Automaton", AUT;
		"States", STAT;
		"Final", FIN;
		"Transitions", TRANS;
	]

	let new_line lexbuf =
	  let lcp = lexbuf.Lexing.lex_curr_p in
	  lexbuf.Lexing.lex_curr_p <- { lcp with
	    Lexing.pos_lnum = lcp.Lexing.pos_lnum + 1;
	    Lexing.pos_bol = lcp.Lexing.pos_cnum;
	  }
	;;

}
let pref = '0'['b' 'B' 'o' 'O' 'x' 'X' ]
let ch = ['A' - 'Z' 'a' - 'z' '_']
let dig = ['0' - '9']
rule
token =	parse	  [ ' ' '\t' '\r' ]	{ token lexbuf }     (* skip blanks *)
		| [ '\n' ]		{ let _ = new_line lexbuf in token lexbuf }
		| eof			{ EOF }
		| ch(ch|dig)* as id	{ try Hashtbl.find keywords id with Not_found -> ID (id) }
		| (pref?dig+) as num	{ NUM (int_of_string num) }
		| ':'			{ COL }
		| ','			{ COM }
		| "->"			{ LA }
		| '('			{ LPAR }
		| ')'			{ RPAR }

