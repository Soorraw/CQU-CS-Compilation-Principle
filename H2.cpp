/*
Exp -> AddExp

    Exp.v

Number -> IntConst | floatConst

PrimaryExp -> '(' Exp ')' | Number
    PrimaryExp.v

UnaryExp -> PrimaryExp | UnaryOp UnaryExp
    UnaryExp.v

UnaryOp -> '+' | '-'

MulExp -> UnaryExp { ('*' | '/') UnaryExp }
    MulExp.v

AddExp -> MulExp { ('+' | '-') MulExp }
    AddExp.v
*/
#include<map>
#include<cassert>
#include<string>
#include<iostream>
#include<vector>
#include<set>
#include<queue>

#define TODO assert(0 && "TODO")
// #define DEBUG_DFA
// #define DEBUG_PARSER

// enumerate for Status
enum class State {
    Empty,              // space, \n, \r ...
    IntLiteral,         // int literal, like '1' '01900', '0xAB', '0b11001'
    op                  // operators and '(', ')'
};
std::string toString(State s) {
    switch (s) {
    case State::Empty: return "Empty";
    case State::IntLiteral: return "IntLiteral";
    case State::op: return "op";
    default:
        assert(0 && "invalid State");
    }
    return "";
}

// enumerate for Token type
enum class TokenType{
    INTLTR,        // int literal
    PLUS,        // +
    MINU,        // -
    MULT,        // *
    DIV,        // /
    LPARENT,        // (
    RPARENT,        // )
};
std::string toString(TokenType type) {
    switch (type) {
    case TokenType::INTLTR: return "INTLTR";
    case TokenType::PLUS: return "PLUS";
    case TokenType::MINU: return "MINU";
    case TokenType::MULT: return "MULT";
    case TokenType::DIV: return "DIV";
    case TokenType::LPARENT: return "LPARENT";
    case TokenType::RPARENT: return "RPARENT";
    default:
        assert(0 && "invalid token type");
        break;
    }
    return "";
}

// definition of Token
struct Token {
    TokenType type;
    std::string value;
};

// definition of DFA
struct DFA {
    /**
     * @brief constructor, set the init state to State::Empty
     */
    DFA();
    
    /**
     * @brief destructor
     */
    ~DFA();
    
    // the meaning of copy and assignment for a DFA is not clear, so we do not allow them
    DFA(const DFA&) = delete;   // copy constructor
    DFA& operator=(const DFA&) = delete;    // assignment

    /**
     * @brief take a char as input, change state to next state, and output a Token if necessary
     * @param[in] input: the input character
     * @param[out] buf: the output Token buffer
     * @return  return true if a Token is produced, the buf is valid then
     */
    bool next(char input, Token& buf);

    /**
     * @brief reset the DFA state to begin
     */
    void reset();

private:
    State cur_state;    // record current state of the DFA
    std::string cur_str;    // record input characters
};


DFA::DFA(): cur_state(State::Empty), cur_str() {}

DFA::~DFA() {}

// helper function, you are not require to implement these, but they may be helpful
bool isoperator(char c) {
	return c == '+' || c == '-' || c == '*' || c == '/' || c == '(' || c == ')';
}

TokenType get_op_type(std::string s) {
	if (s == "+")
        return TokenType::PLUS;
    else if (s == "-")
        return TokenType::MINU;
    else if (s == "*")
        return TokenType::MULT;
    else if (s == "/")
        return TokenType::DIV;
    else if (s == "(")
        return TokenType::LPARENT;
    else if (s == ")")
        return TokenType::RPARENT;

	assert(0 && "invalid operator");
    return TokenType::INTLTR;
}

bool DFA::next(char input, Token& buf) {
    switch (cur_state)
    {
	    case State::Empty:
	        if (isspace(input))
	        {
	            cur_state = State::Empty;
	            return false;
	        }
	        
			if (isalnum(input))
	        {
	            cur_state = State::IntLiteral;
	            cur_str += input;
	        }
	        else if (isoperator(input))
	        {
	            cur_state = State::op;
	            cur_str += input;
	        }
	        else assert(0 && "invalid input");
	        break;
	
	    case State::IntLiteral:
	        if (isalnum(input))
	            cur_str += input;
	        else
	        {
	            buf.type = TokenType::INTLTR;
	            buf.value = cur_str;
	            cur_str = "";
	            if (isspace(input))
	            {
	                cur_state = State::Empty;
	            }
	            else if (isoperator(input))
	            {
	                cur_str += input;
	                cur_state = State::op;
	            }
	            return true;
	        }
	        break;
	
	    case State::op:
	        buf.type = get_op_type(cur_str);
	        buf.value = cur_str;
	        cur_str = "";
	        if (isspace(input))
	            cur_state = State::Empty;
	        else if (isoperator(input))
	        {
	            cur_str += input;
	            cur_state = State::op;
	        }
	        else if (isalnum(input))
	        {
	            cur_state = State::IntLiteral;
	            cur_str += input;
	        }
	        return true;
	    default: 
			assert(0 && "invalid state");
    }
    return false;
}

void DFA::reset() {
    cur_state = State::Empty;
    cur_str = "";
}

// hw2
enum class NodeType {
    TERMINAL,       // terminal lexical unit
    EXP,
    NUMBER,
    PRIMARYEXP,
    UNARYEXP,
    UNARYOP,
    MULEXP,
    ADDEXP,
    NONE
};
std::string toString(NodeType nt) {
    switch (nt) {
    case NodeType::TERMINAL: return "Terminal";
    case NodeType::EXP: return "Exp";
    case NodeType::NUMBER: return "Number";
    case NodeType::PRIMARYEXP: return "PrimaryExp";
    case NodeType::UNARYEXP: return "UnaryExp";
    case NodeType::UNARYOP: return "UnaryOp";
    case NodeType::MULEXP: return "MulExp";
    case NodeType::ADDEXP: return "AddExp";
    case NodeType::NONE: return "NONE";
    default:
        assert(0 && "invalid node type");
        break;
    }
    return "";
}

// tree node basic class
struct AstNode{
    int value;
    NodeType type;  // the node type
    AstNode* parent;    // the parent node
    std::vector<AstNode*> children;     // children of node

    /**
     * @brief constructor
     */
    AstNode(NodeType t = NodeType::NONE, AstNode* p = nullptr): type(t), parent(p), value(0) {} 

    /**
     * @brief destructor
     */
    virtual ~AstNode() {
        for(auto child: children) {
            delete child;
        }
    }

    // rejcet copy and assignment
    AstNode(const AstNode&) = delete;
    AstNode& operator=(const AstNode&) = delete;
};

// definition of Parser
// a parser should take a token stream as input, then parsing it, output a AST
struct Parser {
    uint32_t index; // current token index
    const std::vector<Token>& token_stream;

    /**
     * @brief constructor
     * @param tokens: the input token_stream
     */
    Parser(const std::vector<Token>& tokens): index(0), token_stream(tokens) {}

    /**
     * @brief destructor
     */
    ~Parser() {}
    
    /**
     * @brief creat the abstract syntax tree
     * @return the root of abstract syntax tree
     */
    AstNode* get_abstract_syntax_tree() {
		return matchExp();    
    }

    // u can define member funcition of Parser here
	AstNode* matchExp()
	{
		AstNode *it = new AstNode(NodeType::EXP);
		AstNode *fst = matchAddExp();
		it->children.push_back(fst);
		fst->parent = it;
		return it;
	}
	AstNode* matchAddExp()
	{
		AstNode *it = new AstNode(NodeType::ADDEXP);
		AstNode *fst = matchMulExp();
		it->children.push_back(fst);
		fst->parent = it;
		
		while (index < token_stream.size() && (token_stream[index].type == TokenType::PLUS || token_stream[index].type == TokenType::MINU))
		{
			AstNode *op = new AstNode(NodeType::TERMINAL);
			op->value = static_cast<int>(token_stream[index].type);
			index++;
			AstNode *mul = matchMulExp();
			it->children.push_back(op);
			it->children.push_back(mul);
			op->parent = it;
			mul->parent = it;
		}
		return it;
	}
	AstNode* matchMulExp()
	{
		AstNode *it = new AstNode(NodeType::MULEXP);
		AstNode *fst = matchUnaryExp();
		fst->parent = it;
		it->children.push_back(fst);

		while (index < token_stream.size() && (token_stream[index].type == TokenType::MULT || token_stream[index].type == TokenType::DIV))
		{
			AstNode *op = new AstNode(NodeType::TERMINAL);
			op->value = static_cast<int>(token_stream[index].type);
			index++;
			AstNode *unary = matchUnaryExp();
			it->children.push_back(op);
			it->children.push_back(unary);
			op->parent = it;
			unary->parent = it;
		}
		return it;		
	}

	AstNode* matchUnaryExp()
	{
		AstNode *it = new AstNode(NodeType::UNARYEXP);
		AstNode *fst;
		if (token_stream[index].type == TokenType::PLUS || token_stream[index].type == TokenType::MINU)
		{
			AstNode *op = matchUnaryOp();
			it->children.push_back(op);
			op->parent = it;
			fst = matchUnaryExp();
		}
		else
		{
			fst = matchPrimaryExp();
		}
		it->children.push_back(fst);
		fst->parent = it;
		return it;		
	}

	AstNode* matchUnaryOp()
	{
		AstNode *it = new AstNode(NodeType::UNARYOP);
		if (token_stream[index].type != TokenType::PLUS && token_stream[index].type != TokenType::MINU)
		{
			assert(0 && "invalid token type in function derUnaryOp");
			return nullptr;
		}
		AstNode *fst = new AstNode(NodeType::TERMINAL);
		it->children.push_back(fst);
		fst->parent = it;
		fst->value = static_cast<int>(token_stream[index].type);
		index++;
		return it;		
	}

	AstNode* matchPrimaryExp()
	{
		AstNode *it = new AstNode(NodeType::PRIMARYEXP);
		AstNode *fst;
		if (token_stream[index].type == TokenType::LPARENT)
		{
			index++;
			fst = matchExp();
			if (token_stream[index].type == TokenType::RPARENT)
			{
				index++;
			}
			else
			{
				assert(0 && "Missing RPARENT");
			}
		}
		else
		{
			fst = matchNumber();
		}
		it->children.push_back(fst);
		fst->parent = it;
		return it;		
	}

	AstNode *matchNumber()
	{
		AstNode *it = new AstNode(NodeType::NUMBER);
		if (token_stream[index].type != TokenType::INTLTR)
		{
			assert(0 && "invalid token type in function derNumber");
			return nullptr;
		}

		AstNode *fst = new AstNode(NodeType::TERMINAL);
		std::string val = token_stream[index].value;
		if (val.length() >= 3 && val[0] == '0' && (val[1] == 'x' || val[1] == 'X'))
		{
			fst->value = std::stoi(val.substr(2), nullptr, 16);
		}
		else if (val.length() >= 3 && val[0] == '0' && (val[1] == 'b' || val[1] == 'B'))
		{
			fst->value = std::stoi(val.substr(2), nullptr, 2);
		}
		else if (val.length() >= 2 && val[0] == '0')
		{
			fst->value = std::stoi(val, nullptr, 8);
		}
		else
		{
			fst->value = std::stoi(val, nullptr, 10);
		}
		it->children.push_back(fst);
		fst->parent = it;
		index++;
		return it;
	}
	
	int get_value(AstNode* it)
	{
		int res;
		switch (it->type)
		{
			case NodeType::TERMINAL:
				return it->value;
	
			case NodeType::EXP:
			case NodeType::UNARYOP:
			case NodeType::PRIMARYEXP:
			case NodeType::NUMBER:
				return it->value = get_value(it->children[0]);
	
			case NodeType::UNARYEXP:
				if (it->children[0]->type == NodeType::PRIMARYEXP)
					it->value = get_value(it->children[0]);
				else
				{
					int op = (get_value(it->children[0]) == static_cast<int>(TokenType::PLUS))? 1 : -1;
					it->value = op*get_value(it->children[1]);
				}
				return it->value;
				
			case NodeType::MULEXP:
			case NodeType::ADDEXP:
				res = get_value(it->children[0]);
				for (int i = 1; i < it->children.size(); i += 2)
				{
					int val = it->children[i]->value;
					if (val == static_cast<int>(TokenType::MULT))
					{
						res *= get_value(it->children[i + 1]);
					}
					else if(val == static_cast<int>(TokenType::DIV))
					{
						res /= get_value(it->children[i + 1]);
					}
					else if(val == static_cast<int>(TokenType::PLUS))
					{
						res += get_value(it->children[i + 1]);						
					}
					else
					{
						res -= get_value(it->children[i + 1]);							
					}
				}
				it->value = res;
				return it->value;
							
			default:
				assert(0 && "invalid NodeType");
				return 0;
		}		
	}
    
};

// u can define funcition here


int main(){
    std::string stdin_str;
    std::getline(std::cin, stdin_str);
    stdin_str += "\n";
    DFA dfa;
    Token tk;
    std::vector<Token> tokens;
    for (size_t i = 0; i < stdin_str.size(); i++) {
        if(dfa.next(stdin_str[i], tk)){
            tokens.push_back(tk); 
        }
    }

    // hw2
    Parser parser(tokens);
    auto root = parser.get_abstract_syntax_tree();
    // u may add function here to analysis the AST, or do this in parsing
    // like get_value(root);
    	parser.get_value(root);

    std::cout << root->value;

    return 0;
}

