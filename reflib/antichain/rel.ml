let no rel = Array.length rel.(0)

let copy rel = Array.map Array.copy rel

let blocks_in_rel rel (b1,b2) =
	List.for_all (fun q -> 
		List.for_all (fun r -> 
			rel.(q).(r)
		) b2
	) b1

let is_reflexive rel =
   let result = ref true in
   let l = Array.length rel in
   for i=0 to l-1 do
		if !result then result:=rel.(i).(i)
   done
   ;!result



let is_transitive rel =
   let result = ref true in
   let l = Array.length rel in
   for i=0 to l-1 do
      for j=0 to l-1 do
         for k=0 to l-1 do
            if rel.(i).(j) && rel.(j).(k) && not rel.(i).(k) then (
(*                     Printf.printf "\n%d,%d,%d\n" i j k;*)
               result:=false
            )
         done
      done
   done
   ;!result

let is_symmetric rel =
   let result = ref true in
   Array.iteri (fun i _ ->
      Array.iteri (fun j _ ->
         if rel.(i).(j)<>rel.(j).(i) then
            result:=false
      ) rel
   ) rel
   ;!result

let symmetric rel =
   let l = Array.length rel in
   let eq = Array.make_matrix l l false in
   for i = 0 to l-1 do
      for j = 0 to l-1 do
         eq.(i).(j) <- rel.(i).(j) && rel.(j).(i)
      done 
   done
   ; eq

let complement rel =
   let l = Array.length rel in
   let res = Array.make_matrix l l false in
   for i=0 to l-1 do
      for j=0 to l-1 do
         res.(i).(j)<-not rel.(i).(j)
      done 
   done;
   res

let contains rel1 rel2 =
   let l = Array.length rel2 in
   let contains = ref true in
   for i=0 to l-1 do 
      for j=0 to l-1 do 
         if not rel1.(i).(j) && rel2.(i).(j) then 
            contains:=false
      done
   done;
   !contains
         
let transitive_wrt refl keep =
   let tr = Array.map (Array.copy) refl in 

(*   if not (is_transitive keep) then failwith"get_transitive_wrt0"; *)
(*   if not (contains tr keep) then failwith"get_transitive_wrt00"; *)

   let l = Array.length tr in 
   let transitivity_broken i j k = tr.(i).(j) && tr.(j).(k) && not tr.(i).(k) in
   let rec recursive_cut i j k =
      let (x,y) = 
         if not keep.(i).(j) then (i,j)
         else if not keep.(j).(k) then (j,k)
         else failwith"recursive_cut"
(*			else (j,k)*)
      in
      tr.(x).(y)<-false;
      for n=0 to l-1 do
         if transitivity_broken x n y then
            recursive_cut x n y
      done
   in
   for i=0 to l-1 do 
      for j=0 to l-1 do 
         for k=0 to l-1 do
            if transitivity_broken i j k then
               recursive_cut i j k
         done
      done
   done;

(*   if not (contains tr keep) then failwith"get_transitive_wrt1"; *)
(*   if not (is_transitive tr) then failwith"get_transitive_wrt2"; *)

   tr

let cardinality matrix = 
   Array.fold_left (fun n row ->
      n + (Array.fold_left (fun m r ->
         m + (if r then 1 else 0)
      ) 0 row)
   ) 0 matrix 

(*	composition of rel1 and rel2^-1*)
let composition rel1 rel2 =
   let l = Array.length rel1 in
   let rel = Array.make_matrix l l false in
   for i = 0 to l-1 do
      for j = 0 to l-1 do
         let exists_t = ref false in
         for t = 0 to l-1 do
            exists_t:= !exists_t || (rel1.(i).(t) && rel2.(j).(t));
         done;
         rel.(i).(j) <- !exists_t;
      done 
   done
   ; rel

let intersection rel1 rel2 = 
   Array.mapi (fun i _ ->
      Array.mapi (fun j _ ->
         rel1.(i).(j) && rel2.(i).(j)
      )rel1
   )rel1

let inversion rel = 
   let n = Array.length rel in
   let res = Array.make_matrix n n false in
   for i=0 to n-1 do
      for j=0 to n-1 do
         res.(i).(j)<-rel.(j).(i)
      done
   done
   ;res

let are_indenpendent rel1 rel2 =
   let result = ref true in
   let len = Array.length rel1 in
   for i = 0 to len-1 do 
      for j = 0 to len-1 do 
         for k = 0 to len-1 do  
            if rel1.(i).(k) && rel2.(j).(k) then (
               let s_exists = ref false in
               for s = 0 to len-1 do 
                  s_exists := !s_exists || (rel1.(s).(j) && rel2.(s).(i))
               done;
               if not !s_exists then (
(*						Printf.printf "%d\%d/%d\n" i k j;*)
                  result:=false
               )
            )
         done;
      done;
   done
;!result

(*compose h and s^-1, get usefull edges*)
let usefull h s =
	let n = Array.length h in 
	let c = Array.make_matrix n n false in 
	for i=0 to n-1 do 
		for j=0 to n-1 do
			for k=0 to n-1 do 
				if h.(i).(k) && s.(j).(k) then c.(i).(j) <- true
			done
		done 
	done;
(*	Mess.print_bool_matrix c;*)
(*	print_newline();*)
(*	print_newline();*)
	let u = Array.map Array.copy c in
	for i=0 to n-1 do 
		for j=0 to n-1 do
			for k=0 to n-1 do 
				if h.(i).(j) && u.(j).(k) && not c.(i).(k) then u.(j).(k) <- false;
				if u.(i).(j) && h.(j).(k) && not c.(i).(k) then u.(i).(j) <- false
			done
		done 
	done;
(*	if not (is_transitive u) then failwith"U is not transitive!";*)
	u

(*erases members of b that can not satisfy 'f^-1 composed with b^T included in b'*)
let last_step_fragment f b =
	let n = no f in
	let bt = Array.map Array.copy b in
	for i=0 to n-1 do 
		for j=0 to n-1 do
			for k=0 to n-1 do 
				if f.(j).(i) && bt.(j).(k) && not b.(i).(k) then bt.(i).(k) <- false;
			done
		done 
	done;
	transitive_wrt bt (inversion f)


(*erases members of b that can not satisfy 'f^-1 composed with b^T included in b'*)
let what_can_be_in_the_final_preorder f b =
	let n = no f in
	let bf = copy b in
	for i=0 to n-1 do 
		for j=0 to n-1 do
			for k=0 to n-1 do 
				if f.(j).(i) && bf.(j).(k) && not b.(i).(k) then bf.(j).(k) <- false;
			done
		done 
	done;
	bf

let identity n = 
        let id = Array.make_matrix n n false in
        for i=0 to n-1 do id.(i).(i) <- true done;
        id


let above rel q = fst (
	Array.fold_left (
		fun (above_list,i) value -> 
			if value then (i::above_list,i+1) else (above_list,i+1)
	)  ([],0) rel.(q)
)

let restrict rel n = 
	Array.map (fun a -> Array.sub a 0 n) (Array.sub rel 0 n)

let set_square rel s d value =
	for i=s to s+d-1 do
		for j=s to s+d-1 do
			rel.(i).(j)<-value
		done
	done

(*let print = Mess.print_bool_matrix*)


(* Given a relation R and a transitive relation T such that T \subseteq R,
 * I want to erase everything from R what can not be in any transitive fragment of R 
 * containing T*)
let erase_useless big_rel trans_frag =
   let fragment = copy big_rel in 

   if not (is_transitive trans_frag) then failwith"erase_useless"; 
   if not (contains fragment trans_frag) then failwith"erase_useless"; 

	let erased = ref [] in
   let n = Array.length big_rel in 
   let rec recursive_cut i k =
		if not fragment.(i).(k) then 
			for j=0 to n-1 do
				if fragment.(i).(j) && trans_frag.(j).(k) then  (
					fragment.(i).(j) <- false; 
					erased := (i,j)::!erased;
					recursive_cut i j
				)
				else if trans_frag.(i).(j) && fragment.(j).(k) then  (
					fragment.(j).(k) <- false; 
					erased := (j,k)::!erased;
					recursive_cut j k
				) 
			done
   in
   for i=0 to n-1 do 
      for j=0 to n-1 do 
			recursive_cut i j
      done
   done;

   if not (contains fragment trans_frag) then failwith"erase_useless"; 

   fragment,!erased

(*	The maximal reflexive fragment of a transitive relation*)
let reflexive_fragment rel = 
	let n = Array.length rel in
	let refl = Array.make_matrix n n false in
	for i=0 to n-1 do
		for j=0 to n-1 do
			if rel.(i).(j) && rel.(j).(i) then refl.(i).(j) <-true
		done
	done;
	refl

(*	given r and transitive t included in r, I want to find the first place 
 *	of nontransitivity of r and the pair that can be erased without touching t*)

let find_first_nontransitivity r t =
(*	if not (is_transitive t) then failwith"find_nontransitivity1";*)
(*	if (is_transitive r) then failwith"find_nontransitivity2";*)
(*	if not (contains r t) then failwith"find_nontransitivity3";*)

	let n = no r in
	let i,res = ref 0,ref None in
	while !i<n && !res = None do
		let j = ref 0 in
		while !j<n && !res = None do
			let k = ref 0 in
			while !k<n && !res = None do
				if r.(!i).(!j) && r.(!j).(!k) && not r.(!i).(!k) then (
					res := Some (if t.(!i).(!j) then (!j,!k) else (!i,!j))
				);
				incr k
			done;
			incr j
		done;
		incr i
	done;
	match !res with 
	None -> failwith "find_nontransitivity4"
	| Some p -> p



(* a version, where you try to keep symmetrc links*)
let find_first_nontransitivity2 r t =
(*	if not (is_transitive t) then failwith"find_nontransitivity1";*)
(*	if (is_transitive r) then failwith"find_nontransitivity2";*)
(*	if not (contains r t) then failwith"find_nontransitivity3";*)

	let n = no r in
	let i,res = ref 0,ref None in
	while !i<n && !res = None do
		let j = ref 0 in
		while !j<n && !res = None do
			let k = ref 0 in
			while !k<n && !res = None do
				if r.(!i).(!j) && r.(!j).(!k) && not r.(!i).(!k) then (
					res := 
						if not t.(!j).(!k) && not r.(!k).(!j) then Some (!j,!k) 
						else if not t.(!i).(!j) && not r.(!j).(!i) then Some (!i,!j)					
						else None
				);
				incr k
			done;
			incr j
		done;
		incr i
	done;
	match !res with 
	None -> find_first_nontransitivity r t
	| Some p -> p


let update_cardinality_string ?divider:(div="|") cstr rel = Printf.sprintf "%s%s%d" cstr div (cardinality rel)
