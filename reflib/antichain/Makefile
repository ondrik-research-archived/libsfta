INC = -I ..
OBJ =  fixpointAntichain.cmo incl.cmo algorithms.cmo localsim.cmo rel.cmo merge.cmo
OBJN = fixpointAntichain.cmx incl.cmx algorithms.cmx localsim.cmx rel.cmx merge.cmx

all: $(OBJN)

%.cmo: %.ml
	ocamlc -g $(INC) -c $<

%.cmx: %.ml
	ocamlopt $(INC) -c $<

clean:
	rm -f *.cmo *.cmx *.cmi *.o *~

rebuild: clean all

