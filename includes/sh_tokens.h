/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   header.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ede-ram <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/01/27 13:17:07 by ede-ram           #+#    #+#             */
/*   Updated: 2020/01/27 13:17:09 by ede-ram          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SH_TOKENS_H
# define SH_TOKENS_H

# include <unistd.h>
# include "sh.h"

void		print_all_tokens(t_sh *p, t_token *t, int lvl);
t_toktype	sh_match_tok_op(const char *tok_content, size_t i);
t_toktype	sh_get_res(const char *tok_content);
t_toktype	sh_is_res_word(const char *tok_content, size_t i);
t_toktype	sh_get_tok_type(const char *in, size_t i);
t_token		*sh_get_tok_sub(const char *tok_content);
t_token		*sh_tokenizer(const char *input);
t_token		*sh_init_tok(t_toktype type, const char *content);

#endif
