include ../flags.mk

MODULE = http
OBJ = session.o parser/http_parser.o response/http_response.o
LIB = lib$(MODULE).a

LIBS = liblogger.a # Other modules your library may be dependent on

LDLIBS = -lcriterion

TEST_OBJ = tests/session_test.o tests/parser/http_parser_test.o tests/response/http_response_test.o
TEST_BIN = tests/session_test tests/parser/http_parser_test tests/response/http_response_test

$(MODULE): $(LIB)($(OBJ))

check: $(TEST_OBJ) $(MODULE) $(LIBS)
	$(CC) $< -o $(TEST_BIN) -Wl,--start-group $(LIB) $(addprefix $(SRC_DIR), $(LIBS)) -Wl,--end-group $(LDLIBS)
	$(TEST_BIN)

include ../libs.mk

clean:
	$(RM) $(OBJ) $(LIB) $(TEST_BIN) $(TEST_OBJ) $(addprefix $(SRC_DIR), $(LIBS))

.PHONY: all clean $(MODULE)
