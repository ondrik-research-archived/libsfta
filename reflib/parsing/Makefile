INC = -I ..
OBJ = parser.cmo scanner.cmo
OBJN = parser.cmx scanner.cmx

all: prepare $(OBJN)

%.cmo: %.ml
	ocamlc -g $(INC) -c $<

%.cmx: %.ml
	ocamlopt $(INC) -c $<

prepare: scanner.ml parser.ml

scanner.ml: scanner.mll
	ocamllex scanner.mll

parser.ml: parser.mly
	ocamlyacc -v parser.mly
#	ocamlc -i parser.ml > parser.mli
	rm -f parser.mli
#	ocamlc -c parser.mli

clean:
	rm -f *.cmo *.o *.cmx *.cmi scanner.ml parser.ml parser.mli parser.output *~

rebuild: clean all

