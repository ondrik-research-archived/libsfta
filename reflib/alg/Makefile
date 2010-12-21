INC = -I ..
OBJ = enum.cmo dllist.cmo dll.cmo smartSet.cmo lts.cmo relation.cmo rantapBase.cmo translation2.cmo rantap.cmo rantap3.cmo
OBJN = enum.cmx dllist.cmx dll.cmx smartSet.cmx lts.cmx relation.cmx rantapBase.cmx translation2.cmx rantap.cmx rantap3.cmx

all: $(OBJN)

%.cmo: %.ml
	ocamlc -g $(INC) -c $<

%.cmx: %.ml
	ocamlopt $(INC) -c $<

clean:
	rm -f *.cmo *.cmx *.cmi *.o *~

rebuild: clean all

