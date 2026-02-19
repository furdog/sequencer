#include "sequencer.h"

void sequencer_test_generic(struct sequencer *self)
{
	assert(sequencer_update(self, 0u) == 0u);

	/* Test add entry */
	assert(sequencer_get_entry_count(self) == 0u);
	assert(sequencer_add_entry(self, 100u, 37u) == true);
	assert(sequencer_get_entry_count(self) == 1u);

	/* Test event emited */
	assert(sequencer_update(self, 99u) == 0u);
	assert(sequencer_update(self, 1u) == 37u);
	assert(sequencer_get_entry_count(self) == 1u);

	/* Test cleanup after last event emited */
	assert(sequencer_update(self, 0u) == 0u);
	assert(sequencer_get_entry_count(self) == 0u);

	/* Test no capacity after exceed */
	assert(sequencer_add_entry(self, 0u,   37u) == true);
	assert(sequencer_add_entry(self, 100u, 38u) == true);
	assert(sequencer_add_entry(self, 100u, 39u) == true);
	assert(sequencer_add_entry(self, 100u, 40u) == false);
	assert(sequencer_get_entry_count(self) == 3u);

	/* Test all entries events */
	assert(sequencer_update(self, 0u)   == 37u);
	assert(sequencer_update(self, 100u) == 38u);
	assert(sequencer_update(self, 100u) == 39u);
	assert(sequencer_get_entry_count(self) == 3u);
	assert(sequencer_update(self, 0u) == 0u);
	assert(sequencer_get_entry_count(self) == 0u);
}

int main()
{
	struct sequencer seq;
	struct sequencer_entry entries[3u];
	sequencer_init(&seq, entries, 3u);

	/* Dual test */
	sequencer_test_generic(&seq);
	sequencer_test_generic(&seq);

	return 0;
}
