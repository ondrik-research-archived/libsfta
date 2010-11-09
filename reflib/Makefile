DIRS = parsing alg antichain
INC = -I parsing -I alg -I antichain
OBJ = scanner.cmo parser.cmo generator.cmo interim.cmo enum.cmo dllist.cmo dll.cmo smartSet.cmo lts.cmo relation.cmo translation2.cmo rantapBase.cmo rantap.cmo rantap3.cmo fixpointAntichain.cmo incl.cmo algorithms.cmo localsim.cmo  rel.cmo merge.cmo main.cmo
OBJN = scanner.cmx parser.cmx generator.cmx interim.cmx enum.cmx dllist.cmx dll.cmx smartSet.cmx lts.cmx relation.cmx translation2.cmx rantapBase.cmx rantap.cmx rantap3.cmx fixpointAntichain.cmx incl.cmx algorithms.cmx localsim.cmx  rel.cmx merge.cmx main.cmx 

all: main

#main: generator.cmo interim.cmo parsing alg main.cmo force
#	ocamlc -g $(INC) -o $@ $(OBJ) 

main: generator.cmx interim.cmx parsing alg antichain main.cmx force
	ocamlopt $(INC) -o $@ $(OBJN) 

%.cmo: %.ml
	ocamlc -g $(INC) -c $<

%.cmx: %.ml
	ocamlopt $(INC) -c $<

clean:
	cd alg; $(MAKE) $(MKFLAGS) clean
	cd parsing; $(MAKE) $(MKFLAGS) clean
	cd antichain; $(MAKE) $(MKFLAGS) clean
	rm -f *.cmo *.cmx *.cmi *.o main *~

rebuild: clean all

alg: force
	cd alg; $(MAKE) $(MKFLAGS)

parsing: force
	cd parsing; $(MAKE) $(MKFLAGS)

antichain: force
	cd antichain; $(MAKE) $(MKFLAGS)
force:
