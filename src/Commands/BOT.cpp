#include "Commands/BOT.hpp"
#include "Channel.hpp"
#include <sstream>
#include <cstdlib>
#include <ctime>

CommandBot::CommandBot(const std::string &command, const std::string &params, Server* server, Client& client)
  : ACommand(command, params, server, client) {}

CommandBot::~CommandBot() {}

const std::string phrases[] = {
	"Que a Força esteja com você – sempre.",
	"Com grandes poderes, vêm grandes responsabilidades.",
	"Mesmo a menor pessoa pode mudar o curso do futuro. – O Senhor dos Anéis",
	"Não é apenas um jogo, é uma aventura épica.",
	"Winter is coming... prepare-se e esteja pronto para tudo.",
	"Você é um mago, Harry – e pode mais do que imagina.",
	"Faça ou não faça. Tentativa não há. – Mestre Yoda",
	"A verdadeira jornada começa quando você atinge o nível 100.",
	"A resistência é mais forte do que qualquer feitiço.",
	"Lembre-se: sua triagem não define quem você é. – Divergente",
	"Sempre há esperança. – Jogos Vorazes",
	"Não confie em estranhos, mas tenha coragem quando necessário. – Stranger Things",
	"Aventure-se no desconhecido como um verdadeiro explorador de Tamriel.",
	"Reúna seu grupo — a dungeon espera por heróis.",
	"A chave para vencer está na força da guilda.",
	"Shut up and take my mana!",
	"May the odds be ever in your favor."
	,"Aqui não é lugar para covardes — se aventure!",
	"A Felicidade pode ser encontrada até nas horas mais sombrias, se você se lembrar de acender a luz. – Dumbledore",
	"A vida é como um jogo de chess — você precisa planejar seus movimentos.",
	"Não pare até você alcançar o final da campanha.",
	"Você é mais rápida que um Quicksilver em combate.",
	"Resiliência é seu melhor upgrade.",
	"Ninguém solta a mão de ninguém — Resistência style.",
	"Conquiste cada fase como um gamer veterano.",
	"Heróis não nascem, se formam em batalha.",
	"Seu maior poder é nunca desistir, mesmo contra um Ender Dragon.",
	"Narrativa épica exige coragem épica.",
	"Você tem mais vidas do que imagina.",
	"Tudo que vivemos é combustível para nossa próxima saga.",
	"Não tem final de jogo – apenas novos desafios.",
	"Tudo que é ouro não reluz, mas o seu talento brilha.",
	"A Força não está do seu lado – ela está dentro de você.",
	"Nenhum vilão é páreo quando você joga com inteligência.",
	"Sempre escolha a espada err… o sabre de luz certo.",
	"Para cada puzzle, há uma solução – encontre a sua.",
	"O Coringa não vence quando você se nega a jogar sujo.",
	"Para derrotar o chefe final, você precisa primeiro derrotar seus medos.",
	"Fé em si mesmo é o poder raro de um Jedi.",
	"A grandeza não está nos créditos finais, mas em cada missão cumprida.",
	"O verdadeiro easter egg da vida é aprender algo novo todo dia.",
	"Cada capítulo da sua história pode virar best-seller.",
	"Quando tudo parece perdido, lembre-se: sempre há respawn.",
	"Abeat drop certa pode mudar o rumo da batalha.",
	"Você é o protagonista da sua história — não dê load no passado.",
	"Sua sabedoria é mais forte que qualquer lore milenar.",
	"O universo é vasto — explore-o como em Mass Effect.",
	"Continue jogando até desbloquear todo o seu potencial.",
	"Mesmo sem a Master Sword, você tem coragem de herói.",
	"É perigoso ir sozinho — leve sua determinação.",
	"Em todo RPG, sempre há o NPC que traz a melhor dica.",
	"Não tema o Dark Side — tema desistir de tentar.",
	"Seja persistente como um respawn infinito.",
	"Suas escolhas são mais potentes que qualquer skill tree.",
	"Ótimo, a quest continua — vá em frente."
};

void CommandBot::Execute() const {
	static unsigned int counter = 0;
	srand(time(0) + (++counter * 1000));
	int allphrases = sizeof(phrases) / sizeof(phrases[0]);
	int idx = rand() % allphrases;

	std::string Phrase = phrases[idx];

	std::istringstream iss(this->args);
	std::string channel, test;
	iss >> channel;

	if (channel.empty()) {
		this->client.SendMessage(RPL_PRIVMSG("BOT", this->client.GetNickName(), Phrase), *this->server);
	} else {
		std::map<std::string, Channel*>::const_iterator it = this->server->GetChannel().find(channel);
		if (it != this->server->GetChannel().end()) {
			Channel* chan = it->second;
			chan->BroadcastAllMessage(RPL_PRIVMSG("BOT", chan->GetName(), Phrase), this->server);
		}
	}
}
