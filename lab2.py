from src.lex import lex
from src.yacc import yacc

# 以下是词法分析部分

# 声明所有的token
tokens = ('PLUS', 'MINUS', 'TIMES', 'DIVIDE', 'LPAREN', 'RPAREN',
          'NAME', 'NUMBER')

# 忽略空白
t_ignore = ' \t'

# token正则表达式
t_PLUS = r'\+'
t_MINUS = r'-'
t_TIMES = r'\*'
t_DIVIDE = r'/'
t_LPAREN = r'\('
t_RPAREN = r'\)'
t_NAME = r'[a-zA-Z_][a-zA-Z0-9_]*'


# 识别到token后执行的动作
def t_NUMBER(t):
    r"""\d+"""
    t.value = int(t.value)
    return t


def t_ignore_newline(t):
    r"""\n+"""
    t.lexer.lineno += t.value.count('\n')


# 报错提示
def t_error(t):
    print(f'Illegal character {t.value[0]!r}')
    t.lexer.skip(1)


# 构造词法分析器
lexer = lex()


# 以下是语法分析部分

def p_expression(p):
    """
    expression : term PLUS term
               | term MINUS term
    """
    # p is a sequence that represents rule contents.
    #
    # expression : term PLUS term
    #   p[0]     : p[1] p[2] p[3]
    #
    p[0] = ('binop', p[2], p[1], p[3])


def p_expression_term(p):
    """
    expression : term
    """
    p[0] = p[1]


def p_term(p):
    """
    term : factor TIMES factor
         | factor DIVIDE factor
    """
    p[0] = ('binop', p[2], p[1], p[3])


def p_term_factor(p):
    """
    term : factor
    """
    p[0] = p[1]


def p_factor_number(p):
    """
    factor : NUMBER
    """
    p[0] = ('number', p[1])


def p_factor_name(p):
    """
    factor : NAME
    """
    p[0] = ('name', p[1])


def p_factor_unary(p):
    """
    factor : PLUS factor
           | MINUS factor
    """
    p[0] = ('unary', p[1], p[2])


def p_factor_grouped(p):
    """
    factor : LPAREN expression RPAREN
    """
    p[0] = ('grouped', p[2])


def p_error(p):
    print(f'Syntax error at {p.value!r}')


# Build the parser
parser = yacc()

# Parse an expression
ast = parser.parse('2 * 3 + 4 * (5 - x)')
print(ast)
