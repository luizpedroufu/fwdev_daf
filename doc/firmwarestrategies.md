<!--- cSpell:enable --->

# Estratégias de Construção de Firmware

## Identificação da Complexidade e do Tipo de Projeto

Se você não consegue listar o que precisa ser feito, volte uma casa e elicite melhor os requisitos:

- Estude as características da concorrência.
- Faça engenharia reversa.
- Entreviste os interessados.
- Contrate um consultor.

Se está relativamente claro o que precisa ser feito, algumas perguntas podem ajudar a determinar o nível de complexidade:

- Número de periféricos usados
- Restrições de tempo real
- Restrições de consumo
- Necessidade de tratamento em “paralelo” (sim, não, quantos)
- Recursos computacionais existentes para a solução
- Troca de dados, APIs usadas e dependências externas
- Necessidades de certificações específicos

## Estimativa do Esforço de Desenvolvimento

Existem algumas métricas baseadas em número de linhas de código fonte (SLOC) mas tem mais aplicação em Desktops (a interação com periféricos deixa tudo mais complexo). A maioria das métricas são bem antigas (20 anos ou mais). Enquanto a linguagem C também seja velha, os ambientes e plataformas mudaram muito. Ninguém se arrisca a classificar na área de sistemas embarcados baseado em SLOC, é preciso agregar mais informações sobre o projeto.

- NASA
- COCOMO I / COCOMO II
- Function Point Analysis (FPA)
- COSMIC Function Points
- Complexidade ciclomática
- Halsted's Complexity Measures

