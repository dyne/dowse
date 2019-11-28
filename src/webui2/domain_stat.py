class DomainStat():

    def __init__(self, name, key):
        self.name = name
        self.key = key

        self.accesses = 0
        self.total_accesses = 0
        self.blocked_accesses = 0

        self.subdomains = []
        self.blacklisted = False

        self.parent = None

    def set_accesses(self, accesses, blocked_accesses):
        if self.accesses != 0:
            return

        self.accesses = accesses
        self.blocked_accesses += blocked_accesses

        if not self.parent:
            self.total_accesses += accesses

    def set_parent(self, parent):
        self.parent = parent

    def add_subdomain(self, name, accesses, blocked_accesses):
        subdomain = DomainStat(name, self.key)
        subdomain.set_accesses(accesses, blocked_accesses)
        subdomain.set_parent(self)
        self.subdomains.append(subdomain)
        self.total_accesses += subdomain.accesses
        self.blocked_accesses += blocked_accesses

    def block(self):
        self.blacklisted = True

    def is_blocked(self):
        return self.blacklisted

    def __lt__(self, other):
        if self.accesses == other.accesses:
            return self.name < other.name

        return self.accesses < other.accesses

    def __le__(self, other):
        if self.accesses == other.accesses:
            return self.name <= other.name

        return self.accesses <= other.accesses

    def __eq__(self, other):
        return self.accesses == other.accesses and self.name == self.name

    def __ne__(self, other):
        return self.accesses != other.accesses or self.name != self.name

    def __gt__(self, other):
        if self.accesses == other.accesses:
            return self.name > other.name

        return self.accesses > other.accesses

    def __ge__(self, other):
        if self.accesses == other.accesses:
            return self.name >= other.name

        return self.accesses >= other.accesses
