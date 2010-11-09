
open Dllist;;

type 'a dllist = ('a node_t * int) option;;

let append l x = match l with
  | None -> Some (Dllist.create x, 1)
  | Some (n, s) -> let _ = Dllist.prepend n x in Some (n, s + 1)
;; 

let prepend l x = match l with
  | None -> Some (Dllist.create x, 1)
  | Some (n, s) -> Some (Dllist.prepend n x, s + 1)
;; 

let remove l = match l with
  | None -> raise Dllist.Empty
  | Some (n, s) -> if s == 1 then None else Some (Dllist.drop n, s - 1)
;;

let removeNode l node = match l with
  | None -> raise Dllist.Empty
  | Some (n, s) -> if s == 1 then None else Some (Dllist.drop node, s - 1)
;;

let iter f l = match l with
  | None -> ()
  | Some (n, s) -> Dllist.iter f n
;;

let moveNode src dst node =
	match src with
	  | None -> raise Dllist.Empty
	  | Some (n1, 1) -> (
			match dst with
  		  | None -> (None, src)
	  	  | Some (n2, s2) -> (
					node.next <- n2;
					node.prev <- n2.prev;
					n2.prev.next <- node;
					n2.prev <- node;
					(None, Some (n2, s2 + 1))
  		  )
		)
	  | Some (n1, s1) -> (
 			match dst with
  		  | None -> (
          let head = (if n1 == node then n1.next else n1) in (
						node.next.prev <- node.prev;
						node.prev.next <- node.next;  
					  node.next <- node;
						node.prev <- node;
					  (Some (head, s1 - 1), Some (node, 1))
					)
				)
	  	  | Some (n2, s2) -> (
					let head = (if n1 == node then n1.next else n1) in (
						node.next.prev <- node.prev;
						node.prev.next <- node.next;  
					  node.next <- n2;
					  node.prev <- n2.prev;
					  n2.prev.next <- node;
					  n2.prev <- node;
					  (Some (head, s1 - 1), Some (n2, s2 + 1))
					)
  		  )
			)
;;

let to_list l = match l with
  | None -> []
  | Some (n, s) -> Dllist.to_list n
;;

let dump (l : int dllist) = (
  Printf.printf "dll: ";
	iter (fun i -> Printf.printf "%d " i) l;
	Printf.printf "\n"
);;

let itern f l =
	let rec rec_iter last n =
		f n (Dllist.get n);
		if n != last then
			rec_iter last (Dllist.next n)
  in
	  match l with
      | None -> ()
      | Some (n, s) -> rec_iter (Dllist.prev n) n
;;

let get l = match l with
  | None -> raise Dllist.Empty
  | Some (n, s) -> Dllist.get n
;;

let getn l = match l with
  | None -> raise Dllist.Empty
  | Some (n, s) -> n
;;

let next l = match l with
  | None -> raise Dllist.Empty
  | Some (n, s) -> Some (Dllist.next n, s)
;;

let prev l = match l with
  | None -> raise Dllist.Empty
  | Some (n, s) -> Some (Dllist.prev n, s)
;;

let copy l = match l with
  | None -> None
  | Some (n, s) -> Some (Dllist.copy n, s)
;;

let size l = match l with
  | None -> 0
  | Some (n, s) -> s
;;

let isEmpty l = (l == None);;

let find l x = 
  let rec rec_find n s x = match s with
    | 0 -> raise Not_found
    | s -> if (Dllist.get n) == x then n else rec_find (Dllist.next n) (s - 1) x
  in
    match l with 
      | None -> raise Not_found
      | Some (n, s) -> let r = rec_find n s x in Some (r, s)
;;
