
class relation initial_size = object (self)

	val mutable _data = Array.make_matrix initial_size initial_size true
	val mutable _size = initial_size
	val mutable _index = 0
	val mutable _freeEntries = []

	method newEntry =
	  match _freeEntries with
	    | [] -> (
  		  if _index == _size then (
	  	    _size <- 2*_size;
		    	let a = Array.make_matrix _size _size true in (
			      Array.iteri (fun i x -> Array.iteri (fun j x -> a.(i).(j) <- x) x) _data;
			      _data <- a
			    )
		    );
	      _index <- _index + 1;
		    _index - 1
		  )
		  | h::t -> (
		    _freeEntries <- t;
		    h
		  )

	method deleteEntry x = _freeEntries <- x::_freeEntries;

	method get i j = _data.(i).(j)
	
	method set i j v = _data.(i).(j) <- v
	
	method load m =
		Array.iteri (fun i x -> Array.iteri (fun j y -> _data.(i).(j) <- y) x) m
	
	method dump =
		for i = 0 to _index - 1 do
			for j = 0 to _index - 1 do 
			  Printf.printf "%d" (if _data.(i).(j) then 1 else 0);
			done;
			Printf.printf "\n"
		done
	
end;;
