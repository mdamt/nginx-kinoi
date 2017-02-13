const net = require('net');
var server = net.createServer((socket) => {
  socket.end('omama\n');
}).on('error', (err) => {
  // handle errors here
  throw err;
});

server.listen('/tmp/kinoi.socket', (arg) => {
  console.log(1, arg);
});
