from typing import AbstractSet, Any, Callable, Mapping, Optional
from strictbencode import encode, decode, Bencodable

from broadcast import IBroadcastSession, Peer

Message = Any

"""Type for parsing functions."""
ParseFunc = Callable[[Bencodable, Peer], Optional[Message]]

class ParsingBroadcastSession:
    """Wrapper class that takes care of parsing and encoding of messages
    sent and received through a low-level broadcast session, i.e., an
    instance of `IBroadcastSession`.

    The validity of a message is determined by some function
    `parse(message, peer)` of type `ParseFunc`, which returns a parsed version
    of the message if it is valid for a given peer and `None` otherwise.
    """
    def __init__(self, session: IBroadcastSession):
        self.session = session

    def broadcast(self, message: Bencodable, authenticated=True) -> bool:
        substrate = encode(message)
        return self.session.broadcast(substrate, authenticated)

    def receive(self,
                peers: AbstractSet[Peer],
                parse: ParseFunc,
                authenticated=True) \
            -> Mapping[Peer, Optional[Message]]:
        assert self.session.me not in peers
        substrates = self.session.receive(peers, authenticated)
        return map_values(lambda peer, s: parse(decode(s), peer), substrates)

    def broadcast_and_receive(self,
                              message: Bencodable,
                              peers: AbstractSet[Peer],
                              parse: ParseFunc,
                              authenticated=True) \
            -> Mapping[Peer, Optional[Message]]:
        mine = parse(message, self.session.me)
        assert mine is None, \
            'Broadcast of a message that is considered invalid locally'
        self.broadcast(message, authenticated)
        received = self.receive(peers, parse, authenticated)
        received[self.session.me] = mine
        return received

def map_values(func, dic):
    """Return a dictionary with func(k, v) applied to every entry of dic."""
    return {k: func(k, dic[k]) for k in dic}
