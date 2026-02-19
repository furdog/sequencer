/**
 * @file sequencer.h
 * @brief Simple timed sequencer (millisecond precission), which generates
 *	simple events
 *
 * This file contains the software implementation of the sequencer.
 * The design is hardware-agnostic, requiring an external adaptation layer
 * for hardware interaction.
 *
 * **Conventions:**
 * C89, Linux kernel style, MISRA, rule of 10, No hardware specific code,
 * only generic C and some binding layer. Be extra specific about types.
 *
 * Scientific units where posible at end of the names, for example:
 * - timer_10s (timer_10s has a resolution of 10s per bit)
 * - power_150w (power 150W per bit or 0.15kw per bit)
 *
 * Keep variables without units if they're unknown or not specified or hard
 * to define with short notation.
 *
 * ```LICENSE
 * Copyright (c) 2025 furdog <https://github.com/furdog>
 *
 * SPDX-License-Identifier: 0BSD
 * ```
 *
 * Be free, be wise and take care of yourself!
 * With best wishes and respect, furdog
 */
#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <assert.h>

/** Simple sequencer entry and its functions */
struct sequencer_entry {
	uint16_t timer_ms; /**< How long to wait before firing an event */
	uint8_t  event;    /**< Event to fire */
};

/** Sequencer data structure */
struct sequencer {
	struct sequencer_entry *_entries;
	size_t _capacity;
	size_t _len;

	size_t _iter;

	uint16_t _timer_ms;
};

/** Initializes sequencer, takes entries array and its capacity to work with */
static void sequencer_init(struct sequencer *self,
			   struct sequencer_entry *entries,
			   size_t capacity)
{
	assert(self && entries && (capacity > 0u));

	self->_entries  = entries;
	self->_capacity = capacity;
	self->_len      = 0u;

	self->_iter = 0u;

	self->_timer_ms = 0u;
}

/** Adds entry into a sequencer, return false if no capacity */
static bool sequencer_add_entry(struct sequencer *self, uint32_t timer_ms,
				uint8_t event)
{
	bool has_capacity = true;

	assert(self);

	if (self->_len >= self->_capacity) {
		has_capacity = false;
	} else {
		struct sequencer_entry *entry = &self->_entries[self->_len];

		entry->timer_ms = timer_ms;
		entry->event    = event;
		self->_len     += 1;
	}

	return has_capacity;
}

/** Returns sequencer entry count. */
static size_t sequencer_get_entry_count(struct sequencer *self)
{
	assert(self);

	return self->_len;
}

/** Updates sequencer, returns an event if exceed single entry timer.
 * Advances to the next entry after entry timer exceed.
 * Resets entry count to zero if last entry processed */
static uint8_t sequencer_update(struct sequencer *self, uint32_t delta_time_ms)
{
	uint8_t event = 0u;

	assert(self);

	/* If we have elements in sequence - do checks and emit events */
	if (self->_iter < self->_len) {
		struct sequencer_entry *entry = &self->_entries[self->_iter];

		self->_timer_ms += delta_time_ms;

		if (self->_timer_ms >= entry->timer_ms) {
			self->_timer_ms -= entry->timer_ms;
			self->_iter     += 1;

			event = entry->event;
		}
	/* If it's a last element in a sequence - retry */
	} else if (self->_iter >= self->_len) {
		self->_len      = 0u;
		self->_iter     = 0u;
		self->_timer_ms = 0u;
	} else {}

	return event;
}
