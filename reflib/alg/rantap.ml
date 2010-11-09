
let deref x = match x with Some y -> y | None -> failwith "null pointer exception";;

type block = {
  mutable index: int;
  mutable states: int Dll.dllist;
  mutable remove: int list array;
  mutable relcount: int array array;
  mutable intersection: block option;
  mutable tmp: int Dll.dllist;
};;

let block_create lts states= {
  index = -1;
  states = states;
  remove = Array.make lts#nsymbols [];
  relcount = Array.make_matrix lts#nsymbols lts#nstates 0;
  intersection = None;
	tmp = None;
};;

let block_dump b = (
  Printf.printf "block %d, states: " b.index;
	Dll.iter (fun x -> Printf.printf "%d " x) b.states;
	Printf.printf "\n";
	Array.iteri (fun a x ->
		if (Array.fold_left (fun x y -> x + y) 0 x) > 0 then (
  		Printf.printf "%d: " a;
	  	Array.iteri (fun q y -> Printf.printf "%d" y) x;
		  Printf.printf "\n"
		)
	)	b.relcount
);;

type state_info = {
	mutable b : block;
	mutable n : int Dllist.node_t
};;

class prPair lts = object(self)
  inherit RantapBase.prPairBase lts

	val mutable _partition = []
	val mutable _sindex  =
		let b = block_create lts None	and n = Dllist.create 0 in
		  Array.init lts#nstates (fun i -> { b = b; n = n })
	val mutable _rem_queue = ref []
	val mutable _tmp = Array.make lts#nstates true;
	
	initializer
		let b = _sindex.(0).b in (
      _partition <- [b];
	    b.index <- _relation#newEntry;
      for i = 0 to _lts#nstates - 1 do
				b.states <- Dll.append b.states i;
        _sindex.(i).n <- Dll.getn (Dll.prev b.states)
			done;
		)			

  method dump = (
	  List.iter (fun x -> block_dump x) _partition;
		Printf.printf "relation:\n";
(*		self#dump_part _lts#nstates*)
    _relation#dump
  )

  method dump_part s = (
    for i = 0 to s - 1 do
      for j = 0 to s - 1 do
	      Printf.printf "%d" (if _relation#get _sindex.(i).b.index _sindex.(j).b.index then 1 else 0)
  	  done;
	    Printf.printf "\n"
    done
  )

  method get_sim size = (
		let sim = Array.make_matrix size size false in (
			for i = 0 to size - 1 do
				for j = 0 to size - 1 do
					sim.(i).(j) <- _relation#get _sindex.(i).b.index _sindex.(j).b.index
			  done
			done;
			sim
		)
	)

	method newTask a b =
    _rem_queue := (a, b)::!_rem_queue

	method split (remove : int list) =
    let sl = ref [] in (
      List.iter (fun s ->
        let b = _sindex.(s).b and n = _sindex.(s).n in (
					let (src, dst) = Dll.moveNode b.states b.tmp n in (
						b.states <- src;
						b.tmp <- dst
					);
 	        if Dll.isEmpty b.states then (
		        b.states <- b.tmp;
   				  b.tmp <- None;
			    )
		    )
	    ) remove;
      List.iter (fun s -> 
        let b = _sindex.(s).b in (
					if not (Dll.isEmpty b.tmp) then (
						let bint = block_create _lts b.tmp in (
							_partition <- bint::_partition;
      	      bint.index <- _relation#newEntry;
	  			    Dll.iter (fun s -> _sindex.(s).b <- bint) bint.states;
						  b.intersection <- Some bint;
						  b.tmp <- None;
      			  Array.iteri (fun a x ->
			      	  if x != [] then (
				 	        bint.remove.(a) <- x;
						      self#newTask a bint
				        )
				      ) b.remove
						);
						sl := b::!sl
					)
				)
			) remove;
      List.iter (fun b ->
				let bint = deref b.intersection in (
					b.intersection <- None;
		      List.iter (fun c ->
						_relation#set c.index bint.index (_relation#get c.index b.index);
						_relation#set bint.index c.index (_relation#get b.index c.index)
					) _partition;
	        for a = 0 to _lts#nsymbols - 1 do
					  for x = 0 to _lts#nstates - 1 do
		          bint.relcount.(a).(x) <- b.relcount.(a).(x)
  					done
					done
			  )
		  ) !sl
		)

	method fakeSplit (remove : int list) =
    List.iter (fun s ->
      let b = _sindex.(s).b and n = _sindex.(s).n in (
  			let (src, dst) = Dll.moveNode b.states b.tmp n in (
					b.states <- src;
					b.tmp <- dst
				);
 	      if Dll.isEmpty b.states then (
	        b.states <- b.tmp;
  			  b.tmp <- None;
		    )
	    )
	  ) remove;
    List.iter (fun s -> 
      let b = _sindex.(s).b in (
	  		if not (Dll.isEmpty b.tmp) then
					let bint = block_create _lts b.tmp in (
						_partition <- bint::_partition;
     	      bint.index <- _relation#newEntry;
	  		    Dll.iter (fun s -> _sindex.(s).b <- bint) bint.states;
					  b.tmp <- None;
					)
			)
		) remove;

  method init =
    Array.iteri (fun a pre_a ->
			let post_a = _lts#data_post a in
        List.iter (fun b ->
				  for i = 0 to _lts#nstates - 1 do
					  _tmp.(i) <- true
  				done;
          let remove = ref [] in (
	          List.iter (fun c ->
		          if _relation#get b.index c.index then
		            Dll.iter (fun r -> List.iter (fun q -> _tmp.(q) <- false) pre_a.(r)) c.states
  			    ) _partition;
    	      Array.iteri (fun i x -> if x then remove := i::!remove) _tmp;
		  	    if !remove != [] then (
			        b.remove.(a) <- !remove;
				      self#newTask a b
  			    )
	  		  );
		  		for v = 0 to _lts#nstates - 1 do
			      let x = ref 0 in (
				      List.iter (fun q ->
			  	  	  if _relation#get b.index _sindex.(q).b.index then
		  			  	  incr(x)
  	  			  ) post_a.(v);
	  	  			b.relcount.(a).(v) <- !x
 		  		  )
				  done
        ) _partition;
    ) _lts#data_pre
		
	method oneRound a b =
    let remove = b.remove.(a) in (
(*			List.iter (fun s -> Printf.printf "%d " s) remove;
			Printf.printf "\n";*)
      b.remove.(a) <- [];
      let bprev = Dll.to_list b.states in (
        self#split remove;
				for i = 0 to lts#nstates - 1 do
					_tmp.(i) <- false
				done;
        List.iter (fun x -> _tmp.(x) <- true) remove;
        let removelist = ref [] in (
 	        List.iter (fun d ->
  	        let tmp = ref true in (
	  	        Dll.iter (fun s -> tmp := !tmp && _tmp.(s)) d.states;
	            if !tmp then
		            removelist := d::!removelist
			      )
			    ) _partition;
					for i = 0 to (List.length _partition) - 1 do
						_tmp.(i) <- true
					done;
          List.iter (fun x ->
            List.iter (fun y ->
              let c = _sindex.(y).b in (
                if _tmp.(c.index) then (
                  _tmp.(c.index) <- false;
	                List.iter (fun d ->
	                  if _relation#get c.index d.index then (
	                    _relation#set c.index d.index false;
				    					Array.iteri (fun a pre_a ->
		                    Dll.iter (fun y ->
		                      List.iter (fun x ->
		                        c.relcount.(a).(x) <- c.relcount.(a).(x) - 1;
			                      if c.relcount.(a).(x) == 0 then (
								              if c.remove.(a) == [] then
			                          self#newTask a c;
			                        c.remove.(a) <- x::c.remove.(a)
				                    )
 			                    ) pre_a.(y)
  		                  ) d.states
					  					) lts#data_pre
	                  )
	                ) !removelist
                )
	            )
	          ) lts#data_pre.(a).(x)
          ) bprev
  	    )
	    )
	  )
		
	method compute =
    while !_rem_queue != [] do
      let (a, b) = List.hd !_rem_queue in (
(*				self#dump;*)
        _rem_queue := List.tl !_rem_queue;
	      self#oneRound a b
(*				Printf.printf "block count %d\n" (Dll.size _partition)*)
      )
    done

end;;
