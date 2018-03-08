const server = require('server');
const corsExpress = require('cors')({
  origin: ['http://localhost:1234']
});

const words = require('./words');
const cors = server.utils.modern(corsExpress);

server({ port: 1235 }, cors, ctx => server.reply.json(words));
