/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sh_tokens.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: thdelmas <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/04 15:13:46 by thdelmas          #+#    #+#             */
/*   Updated: 2019/06/01 07:11:59 by ede-ram          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SH_TOKENS
# define SH_TOKENS
typedef enum	e_toktype
{
	SH_NULL = 0,
	SH_EOS,
	SH_EOF,
	SH_WORD,
	SH_ASSIGN_WORD,
	SH_NAME,
	SH_NEWLINE,
	SH_IO_NUMBER,
	SH_LESS,
	SH_GREAT,
	SH_AND,
	SH_OR,
	SH_SEMI,
	SH_AND_IF,
	SH_OR_IF,
	SH_DSEMI,
	SH_DLESS,
	SH_DGREAT,
	SH_LESSAND,
	SH_GREATAND,
	SH_LESSGREAT,
	SH_DLESSDASH,
	SH_CLOBBER,
	SH_IF,
	SH_THEN,
	SH_ELSE,
	SH_ELIF,
	SH_FI,
	SH_DO,
	SH_DONE,
	SH_CASE,
	SH_ESAC,
	SH_WHILE,
	SH_UNTIL,
	SH_FOR,
	SH_LBRACE,
	SH_RBRACE,
	SH_BANG,
	SH_IN,
	SH_FUNC,
	SH_LSUBSH,
	SH_RSUBSH,
	SH_LSUBSH_EXP,
	SH_RSUBSH_EXP,
	SH_LPARAM_EXP,
	SH_RPARAM_EXP,
	SH_LARITH_EXP,
	SH_RARITH_EXP,
	SH_GROUP_TOKEN,
	SH_BRACES
}				t_toktype;
/*
typedef union		u_toksub
{
	char			*str;
	struct s_token  *toklst;
}					t_toksub;

typedef struct      s_token
{
	t_toktype		type;
	t_toksub		sub;
	struct s_token  *next;
}                   t_token;
*/
typedef struct      s_token
{
	int				type;
	struct s_token	*sub;
	char			*content;
	struct s_token  *next;
}                   t_token;

#endif
