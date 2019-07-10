#include "t_token.h"
#include "libft.h"

int			is_word_char(char c) // exhaustive? all operators
{
	if (c == '\n' || c == '<'
			|| c == '>' || c == '&'
			|| c == '|' || c == ' '
			|| c == '\t' || c == ';')
		return (0);
	return (1);
}

int			read_n_skip_word(t_tokenize_tool *t)
{
	int	type;
	int	escaped;
	int	n;
	int	tmp;

	escaped = 0;
	n = 0;
	while (t->input[t->i])
	{
		if (escaped)
			escaped--;
		if (!escaped && t->input[t->i] == '\\')
			escaped = 2;
		if (!escaped && !is_word_char(t->input[t->i]))
			return (n);
		tmp = t->i;
		if (!escaped && (type = read_skip_opening_char(t)))
		{
			skip_ending_char(t, type);
			n += t->i - tmp;
		}
		else
		{
			t->i++;
			n++;
		}
	}
	return (n);
}

int			is_io_nb(t_tokenize_tool *t)
{
	int	j;

	j = 0;
	while (ft_isdigit(t->input[t->i + j]))
		j++;
	if (t->input[t->i + j] == '<' || t->input[t->i + j] == '>')
		return (j);
	return (0);
}

t_toktype		fill_redirection(t_tokenize_tool *t, t_token **p_actual)
{
	int	word_begin;

	forward_blanks(t);
	word_begin = t->i;
	if (!read_n_skip_word(t))
	{
		printf("GRAMMAR ERROR: expected WORD after redirection_operator at -%.10s\n", t->input + t->i - ((t->i - 4 > -1) ? 4 : 2));
		return (SH_SYNTAX_ERROR);
	}
	(*p_actual)->sub = create_token_n(SH_WORD, t->input + word_begin, t->i - word_begin);
	return (0);
}

t_toktype		treat_redirection(t_tokenize_tool *t, t_token **p_actual, int len)
{
	t_toktype	type;

	(*p_actual)->next = create_token_n(0, t->input + t->i, len);
	t->i += len;
	type = read_n_skip_operator(t);
	*p_actual = (*p_actual)->next;
	(*p_actual)->type = type;
	return (fill_redirection(t, p_actual));
}

t_toktype	tokenize_reserved_word(t_tokenize_tool *t, t_token **p_actual, t_toktype type)
{
	if (is_compound(type))
	{
		if (!((*p_actual)->next = tokenize_compound(t, type)))
			return (SH_SYNTAX_ERROR);
	}
	else
		(*p_actual)->next = create_token(type, 0);
	return (0);
}

t_toktype	treat_word(t_tokenize_tool *t, t_token **p_actual, t_toktype actual_compound)
{
	int			word_begin;
	t_toktype	type;
	int			len;

	if ((len = is_io_nb(t)))
	{
		if (treat_redirection(t, p_actual, len) == SH_SYNTAX_ERROR)
			return (SH_SYNTAX_ERROR);
		return (0);
	}
	//if next is ( )
	//	treat_function_def
	word_begin = t->i;
	if (read_n_skip_word(t))
	{
		if ((type = word_is_actual_terminator(t->input + word_begin, t->i - word_begin, actual_compound)) && t->word_nb == 1)
			return (type);
		if (t->word_nb == 1 && (type = word_is_reserved(t->input + word_begin, t->i - word_begin)))
		{
			if (tokenize_reserved_word(t, p_actual, type) == SH_SYNTAX_ERROR)
				return (SH_SYNTAX_ERROR);
		}
		else
		{
			(*p_actual)->next = create_token_n(SH_WORD, t->input + word_begin, t->i - word_begin);
			t->word_nb++;
		}
		*p_actual = (*p_actual)->next;
	}
	return (0);
}

void	treat_input(t_tokenize_tool *t, t_toktype actual_compound, t_toktype *terminator, t_token **p_actual)
{
	forward_blanks(t);
	while (t->input[t->i] == '\n')
	{
		t->i++;
		(*p_actual)->next = create_token(SH_NEWLINE, 0);
		*p_actual = (*p_actual)->next;
		t->word_nb = 1;
	}
	if (!(*terminator = treat_operator(t, p_actual, actual_compound)))
		*terminator = treat_word(t, p_actual, actual_compound);
}

t_token		*recursive_tokenizer(t_tokenize_tool *t, t_toktype actual_compound, t_toktype *terminator)
{
	t_token	*origin;
	t_token	*actual;

	origin = create_token(0, 0);
	actual = origin;
	*terminator = 0;
	while (!*terminator && t->input[t->i])
		treat_input(t, actual_compound, terminator, &actual);
	if (*terminator == SH_SYNTAX_ERROR)
	{
		//free all
		return (0);
	}
	actual = origin->next;
	delete_token(origin);
//	if (!actual)
//		return (create_token(0, 0));
	return (actual);
}

t_token		*tokenize_input(const char *input) //set p->container_stack && p->syntax_error
{
	t_token			*ast;
	t_toktype		terminator;
	t_tokenize_tool	tok_tool;

	tok_tool.input = input;
	tok_tool.i = 0;
	tok_tool.word_nb = 1;
	ast = recursive_tokenizer(&tok_tool, SH_NULL, &terminator);
	return (ast);
}

//{
//	while (1)
//	{
		//read_next_cmd_line
		//if TOKENIZE == UNFINISHED
		//	print ; continue
		//else if TOKENIZE == SYNTAX_ERROR
		//	print ; free ; continue ;
		//exec_script
//	}
//}

//RM COMMENTS (#comm)

//implement word_nb++ in func def
//WTF echo $(echo la`while done`) ; echo lala
//
//
//Parse separately here document