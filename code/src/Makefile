CC = g++ -std=c++11
IT_SRC = Interpreter/
LY_SRC = Interpreter/LEXYACC/
API_SRC = API/
BM_SRC = BufferManager/
IM_SRC = IndexManager/
CM_SRC = CatalogManager/
RM_SRC = RecordManager/

minisql : MiniSQL.cpp MiniSQL.h Global.h\
		  $(LY_SRC)yacc.cpp $(LY_SRC)yacc.hpp $(LY_SRC)lex.cpp \
		  $(IT_SRC)CST.cpp $(IT_SRC)CST.h $(IT_SRC)interpreter.h \
		  $(API_SRC)API.cpp $(API_SRC)API.h \
		  $(BM_SRC)BufferManager.cpp $(BM_SRC)BufferManager.h \
		  $(IM_SRC)BpTree.cpp $(IM_SRC)BpTree.h $(IM_SRC)BpTreeNode.cpp $(IM_SRC)BpTreeNode.h \
		  $(IM_SRC)IndexManager.cpp $(IM_SRC)IndexManager.h\
		  $(CM_SRC)catalogManager.cpp $(CM_SRC)catalogManager.h\
		  $(RM_SRC)recordManager.cpp $(RM_SRC)recordManager.h $(RM_SRC)data.cpp $(RM_SRC)data.h 
	$(CC) -o minisql MiniSQL.cpp $(LY_SRC)yacc.cpp  $(LY_SRC)lex.cpp \
			 $(IT_SRC)CST.cpp $(API_SRC)API.cpp \
			 $(BM_SRC)BufferManager.cpp $(IM_SRC)BpTree.cpp $(IM_SRC)BpTreeNode.cpp \
			 $(IM_SRC)IndexManager.cpp\
			 $(CM_SRC)catalogManager.cpp\
			 $(RM_SRC)recordManager.cpp $(RM_SRC)data.cpp
