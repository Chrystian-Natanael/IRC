#ifndef WHO_HPP
# define WHO_HPP

# include "ACommand.hpp"

class CommandWho : public ACommand {
	private:
		CommandWho(); // Verificar se precisamos manter esse construtor aqui.
	public:
		CommandWho(const std::string &args, Server* server, Client& client);
		CommandWho(const CommandWho& other);
		~CommandWho();
		CommandWho& operator=(const CommandWho& other);
};

/*
	Casos:
	IMPORTANTE: definir se vamos usar wildcards * e ?
	/WHO (sem args) -> pede ao servidor a lista de todos os usuários conectados
		Resposta típica:
			:irc.exemplo.net 352 seuNick * a1 host1.com irc.exemplo.net alice H :0 Alice A.
			:irc.exemplo.net 352 seuNick * b2 host2.net irc.exemplo.net bob G :0 Bob B.
			:irc.exemplo.net 352 seuNick * c3 host3.org irc.exemplo.net carol H :0 Carol C.
			:irc.exemplo.net 315 seuNick * :End of WHO list
	/WHO #canal -> lista todos os usuários presentes no canal #canal
	/WHO #canal o -> lista apenas os operadores no canal #canal
	/WHO nick -> filtra pelo apelido do usuário (definir se vamos usar wildcards * e ?)
	/WHO ~user_name -> filtra pelo user_name (~ é obrigatório?)
	/WHO host -> filtra pelo host ou IP do usuário
	/WHO realName -> filtra pelo nome (Nem todos os servidores permitem busca por real name, por razões de privacidade.)
	Se a máscara bater em qualquer um desses campos, o usuário será incluído na resposta do WHO.

*/

#endif