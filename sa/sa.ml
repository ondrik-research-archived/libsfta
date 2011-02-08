
let deref x = match x with Some y -> y | None -> failwith "null pointer exception";;

class counter nsymbols nstates = object(self)

  val mutable _data = Array.make_matrix nsymbols 0 0 
  val mutable _nsymbols = nsymbols
	val mutable _nstates = nstates
	val mutable _key = Array.make_matrix 0 0 0 
	val mutable _ranges = Array.make 0 0

  method set_key key ranges =
		_key <- key;
		_ranges <- ranges

  method is_zero a q =
		let tmp = _data.(a) in
		  (Array.length tmp == 0) || (tmp.(_key.(a).(q)) == 0)
	
	method incr a q = (
	  if Array.length _data.(a) == 0 then
		  _data.(a) <- Array.make _ranges.(a) 0;
		let tmp = _data.(a) and i = _key.(a).(q) in 
      tmp.(i) <- tmp.(i) + 1;
	) 
	
	method decr a q =
		let tmp = _data.(a) and i = _key.(a).(q) in
      tmp.(i) <- tmp.(i) - 1;

	method get_row a = _data.(a) 
	
	method copy_row a (c : counter) =
	  _data.(a) <- Array.copy (c#get_row a);
	
	method dump = (
  	Array.iteri (fun a x ->
	  	if Array.length x > 0 then (
		    Printf.printf "%d: " a;
		    Array.iteri (fun q y -> Printf.printf "%d" y) x;
		    Printf.printf "\n"
		  )
	  ) _data
	)

end;;

type block = {
  mutable index: int;
  mutable states: int Dll.dllist;
  mutable remove: int list array;
  mutable relcount: counter;
  mutable intersection: block option;
  mutable tmp: int Dll.dllist;
	mutable inset: SmartSet.smartSet;
};;

let block_create lts states = {
  index = -1;
  states = states;
  remove = Array.make lts#nsymbols [];
  relcount = new counter lts#nsymbols lts#nstates;
  intersection = None;
	tmp = None;
	inset = new SmartSet.smartSet lts#nsymbols
};;

let block_dump b = (
  Printf.printf "block %d, states: " b.index;
	Dll.iter (fun x -> Printf.printf "%d " x) b.states;
	Printf.printf "\n";
	b.relcount#dump
);;

type state_info = {
	mutable b : block;
	mutable n : int Dllist.node_t
};;

class prPair lts = object(self)
  inherit SaBase.prPairBase lts

	val mutable _partition = []
	val mutable _sindex  =
		let b = block_create lts None	and n = Dllist.create 0 in
		  Array.init lts#nstates (fun i -> { b = b; n = n })
	val mutable _rem_queue = ref []
	val mutable _tmp = Array.make lts#nstates true;
	val mutable _delta = Array.init lts#nsymbols (fun i -> new SmartSet.smartSet lts#nstates)
	val mutable _delta1 = Array.init lts#nsymbols (fun i -> new SmartSet.smartSet lts#nstates)
	val mutable _key = Array.make_matrix lts#nsymbols lts#nstates (-1)
	val mutable _ranges = Array.make lts#nsymbols 0
	
	initializer
		let b = _sindex.(0).b in (
      _partition <- [b];
	    b.index <- _relation#newEntry;
			b.relcount#set_key _key _ranges;
      for s = 0 to _lts#nstates - 1 do
				b.states <- Dll.append b.states s;
        _sindex.(s).n <- Dll.getn (Dll.prev b.states);
				lts#sym_pre.(s)#iter (fun a -> b.inset#add a)
			done;
		)			

  method dump = (
	  List.iter (fun x -> block_dump x) _partition;
		Printf.printf "relation:\n";
(*		self#dump_part _lts#nstates*)
    _relation#dump
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
  						bint.relcount#set_key _key _ranges;
	  			    Dll.iter (fun s ->
								_sindex.(s).b <- bint;
								_lts#sym_pre.(s)#iter (fun a ->
									b.inset#remove a;
									bint.inset#add a
								)
							) bint.states;
						  b.intersection <- Some bint;
						  b.tmp <- None;
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
					bint.inset#iter (fun a ->
						bint.remove.(a) <- b.remove.(a);
						self#newTask a bint;
(*						let relcount_a = Array.make _lts#nstates 0 in (
						  _delta1.(a)#iter (fun v -> relcount_a.(v) <- b.relcount.(a).(v));
							bint.relcount.(a) <- relcount_a
						)*)
						bint.relcount#copy_row a b.relcount
					);
			  )
		  ) !sl
		)

	method fastSplit (remove : int list) =
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
  						bint.relcount#set_key _key _ranges;
	  			    Dll.iter (fun s ->
								_sindex.(s).b <- bint;
								_lts#sym_pre.(s)#iter (fun a ->
									b.inset#remove a;
									bint.inset#add a
								)
							) bint.states;
						  b.intersection <- Some bint;
						  b.tmp <- None;
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
						bint.relcount#set_key _key _ranges;
	  		    Dll.iter (fun s ->
							_sindex.(s).b <- bint;
							_lts#sym_pre.(s)#iter (fun a ->
								b.inset#remove a;
								bint.inset#add a
							)
						) bint.states;
					  b.tmp <- None;
					)
			)
		) remove;

	method init = (
		for a = 0 to _lts#nsymbols - 1 do
  		_delta.(a)#clear;
	  	_delta1.(a)#clear;
		done;
 		_lts#iter (fun q a r ->
		  _delta.(a)#add r;
	  	_delta1.(a)#add q
  	);
		for a = 0 to _lts#nsymbols - 1 do
			_ranges.(a) <- _delta1.(a)#size;
			let tmp = _key.(a) and i = ref 0 in
			  _delta1.(a)#iter (fun x ->
					tmp.(x) <- !i;
					incr(i)
				)
	  done;
		for a = 0 to _lts#nsymbols - 1 do
	    self#fastSplit _delta1.(a)#states;
	  done;
	  let tmp = Array.make_matrix _lts#nsymbols (List.length _partition) true
		and tmp2 = Array.make_matrix _lts#nsymbols (List.length _partition) true in (
  	  for a = 0 to _lts#nsymbols - 1 do
 				List.iter (fun b ->
			    Dll.iter (fun s ->
			  		if _delta1.(a)#contains s then
		  				tmp2.(a).(b.index) <- false
			  		else
		  				tmp.(a).(b.index) <- false
	  			) b.states					
  			) _partition
			done;
  	  for a = 0 to _lts#nsymbols - 1 do
 	  	  List.iter (fun b ->
  				if tmp.(a).(b.index) then
 	  		    List.iter (fun c ->
			        if tmp2.(a).(c.index) then
					  		_relation#set b.index c.index false
	  	      ) _partition
	      ) _partition
	    done
	  );
    List.iter (fun b ->
			b.inset#iter (fun a ->
				let post_a = _lts#data_post a and pre_a = _lts#data_pre.(a) in (
     		  _delta1.(a)#iter (fun v ->
				    List.iter (fun q ->
			  		  if _relation#get b.index _sindex.(q).b.index then
								b.relcount#incr a v
	  			  ) post_a.(v);
 				  );
			    for i = 0 to _lts#nstates - 1 do
			  	  _tmp.(i) <- _delta1.(a)#contains i
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
	  	    )
  		  )
			)
    ) _partition;
  )

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
										if c.index == d.index then
											failwith "kernel panic: diagonal corrupted";
	                  if _relation#get c.index d.index then (
	                    _relation#set c.index d.index false;
											d.inset#iter (fun a ->
												if c.inset#contains a then
													let pre_a = lts#data_pre.(a) in (
    		                    Dll.iter (fun y ->
		                          List.iter (fun x ->
		                            c.relcount#decr a x;
			                          if c.relcount#is_zero a x then (
			                            if c.remove.(a) == [] then
			                              self#newTask a c;
    			                        c.remove.(a) <- x::c.remove.(a)
		    		                    )
 			                        ) pre_a.(y)
  		                      ) d.states
												  )
											)
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
