
all: sa

sa: interim.ml parser.ml scanner.ml enum.ml dllist.ml dll.ml smartSet.ml lts.ml relation.ml translation2.ml saBase.ml sa.ml main.ml
	ocamlopt -o $@ $^ 

scanner.ml: scanner.mll
	ocamllex scanner.mll

parser.ml: parser.mly
	ocamlyacc parser.mly
	rm -f parser.mli

clean:
	rm -f sa *.cmx *.o *.cmi scanner.ml parser.ml

rebuild: clean all

backup:
	tar -czf sa.tar.gz *.ml *.mly *.mll Makefile