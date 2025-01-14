/*
 * Copyright © 2018  Google, Inc.
 *
 *  This is part of HarfBuzz, a text shaping library.
 *
 * Permission is hereby granted, without written agreement and without
 * license or royalty fees, to use, copy, modify, and distribute this
 * software and its documentation for any purpose, provided that the
 * above copyright notice and the following two paragraphs appear in
 * all copies of this software.
 *
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
 * ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN
 * IF THE COPYRIGHT HOLDER HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 *
 * THE COPYRIGHT HOLDER SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING,
 * BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND THE COPYRIGHT HOLDER HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 *
 * Google Author(s): Garret Rieger, Rod Sheeter, Behdad Esfahbod
 */

#include "hb-subset.hh"
#include "hb-set.hh"

/**
 * hb_subset_input_create_or_fail:
 *
 * Return value: New subset input.
 *
 * Since: 1.8.0
 **/
hb_subset_input_t *
hb_subset_input_create_or_fail ()
{
  hb_subset_input_t *input = hb_object_create<hb_subset_input_t>();

  if (unlikely (!input))
    return nullptr;

  input->unicodes = hb_set_create ();
  input->glyphs = hb_set_create ();
  input->name_ids = hb_set_create ();
  hb_set_add_range (input->name_ids, 0, 6);
  input->name_languages = hb_set_create ();
  hb_set_add (input->name_languages, 0x0409);
  input->layout_features = hb_set_create ();
  input->drop_tables = hb_set_create ();
  input->no_subset_tables = hb_set_create ();
  input->drop_hints = false;
  input->desubroutinize = false;
  input->retain_gids = false;
  input->name_legacy = false;
  input->overlaps_flag = false;
  input->notdef_outline = false;
  input->no_prune_unicode_ranges = false;
  input->retain_all_layout_features = false;
  input->passthrough_unrecognized = false;

  hb_tag_t default_drop_tables[] = {
    // Layout disabled by default
    HB_TAG ('m', 'o', 'r', 'x'),
    HB_TAG ('m', 'o', 'r', 't'),
    HB_TAG ('k', 'e', 'r', 'x'),
    HB_TAG ('k', 'e', 'r', 'n'),

    // Copied from fontTools:
    HB_TAG ('B', 'A', 'S', 'E'),
    HB_TAG ('J', 'S', 'T', 'F'),
    HB_TAG ('D', 'S', 'I', 'G'),
    HB_TAG ('E', 'B', 'D', 'T'),
    HB_TAG ('E', 'B', 'L', 'C'),
    HB_TAG ('E', 'B', 'S', 'C'),
    HB_TAG ('S', 'V', 'G', ' '),
    HB_TAG ('P', 'C', 'L', 'T'),
    HB_TAG ('L', 'T', 'S', 'H'),
    // Graphite tables
    HB_TAG ('F', 'e', 'a', 't'),
    HB_TAG ('G', 'l', 'a', 't'),
    HB_TAG ('G', 'l', 'o', 'c'),
    HB_TAG ('S', 'i', 'l', 'f'),
    HB_TAG ('S', 'i', 'l', 'l'),
  };
  input->drop_tables->add_array (default_drop_tables, ARRAY_LENGTH (default_drop_tables));

  hb_tag_t default_no_subset_tables[] = {
    HB_TAG ('a', 'v', 'a', 'r'),
    HB_TAG ('f', 'v', 'a', 'r'),
    HB_TAG ('g', 'a', 's', 'p'),
    HB_TAG ('c', 'v', 't', ' '),
    HB_TAG ('f', 'p', 'g', 'm'),
    HB_TAG ('p', 'r', 'e', 'p'),
    HB_TAG ('V', 'D', 'M', 'X'),
    HB_TAG ('D', 'S', 'I', 'G'),
    HB_TAG ('M', 'V', 'A', 'R'),
    HB_TAG ('c', 'v', 'a', 'r'),
    HB_TAG ('S', 'T', 'A', 'T'),
  };
  input->no_subset_tables->add_array (default_no_subset_tables,
				      ARRAY_LENGTH (default_no_subset_tables));

  //copied from _layout_features_groups in fonttools
  hb_tag_t default_layout_features[] = {
    // default shaper
    // common
    HB_TAG ('r', 'v', 'r', 'n'),
    HB_TAG ('c', 'c', 'm', 'p'),
    HB_TAG ('l', 'i', 'g', 'a'),
    HB_TAG ('l', 'o', 'c', 'l'),
    HB_TAG ('m', 'a', 'r', 'k'),
    HB_TAG ('m', 'k', 'm', 'k'),
    HB_TAG ('r', 'l', 'i', 'g'),

    //fractions
    HB_TAG ('f', 'r', 'a', 'c'),
    HB_TAG ('n', 'u', 'm', 'r'),
    HB_TAG ('d', 'n', 'o', 'm'),

    //horizontal
    HB_TAG ('c', 'a', 'l', 't'),
    HB_TAG ('c', 'l', 'i', 'g'),
    HB_TAG ('c', 'u', 'r', 's'),
    HB_TAG ('k', 'e', 'r', 'n'),
    HB_TAG ('r', 'c', 'l', 't'),

    //vertical
    HB_TAG ('v', 'a', 'l', 't'),
    HB_TAG ('v', 'e', 'r', 't'),
    HB_TAG ('v', 'k', 'r', 'n'),
    HB_TAG ('v', 'p', 'a', 'l'),
    HB_TAG ('v', 'r', 't', '2'),

    //ltr
    HB_TAG ('l', 't', 'r', 'a'),
    HB_TAG ('l', 't', 'r', 'm'),

    //rtl
    HB_TAG ('r', 't', 'l', 'a'),
    HB_TAG ('r', 't', 'l', 'm'),

    //Complex shapers
    //arabic
    HB_TAG ('i', 'n', 'i', 't'),
    HB_TAG ('m', 'e', 'd', 'i'),
    HB_TAG ('f', 'i', 'n', 'a'),
    HB_TAG ('i', 's', 'o', 'l'),
    HB_TAG ('m', 'e', 'd', '2'),
    HB_TAG ('f', 'i', 'n', '2'),
    HB_TAG ('f', 'i', 'n', '3'),
    HB_TAG ('c', 's', 'w', 'h'),
    HB_TAG ('m', 's', 'e', 't'),
    HB_TAG ('s', 't', 'c', 'h'),

    //hangul
    HB_TAG ('l', 'j', 'm', 'o'),
    HB_TAG ('v', 'j', 'm', 'o'),
    HB_TAG ('t', 'j', 'm', 'o'),

    //tibetan
    HB_TAG ('a', 'b', 'v', 's'),
    HB_TAG ('b', 'l', 'w', 's'),
    HB_TAG ('a', 'b', 'v', 'm'),
    HB_TAG ('b', 'l', 'w', 'm'),

    //indic
    HB_TAG ('n', 'u', 'k', 't'),
    HB_TAG ('a', 'k', 'h', 'n'),
    HB_TAG ('r', 'p', 'h', 'f'),
    HB_TAG ('r', 'k', 'r', 'f'),
    HB_TAG ('p', 'r', 'e', 'f'),
    HB_TAG ('b', 'l', 'w', 'f'),
    HB_TAG ('h', 'a', 'l', 'f'),
    HB_TAG ('a', 'b', 'v', 'f'),
    HB_TAG ('p', 's', 't', 'f'),
    HB_TAG ('c', 'f', 'a', 'r'),
    HB_TAG ('v', 'a', 't', 'u'),
    HB_TAG ('c', 'j', 'c', 't'),
    HB_TAG ('i', 'n', 'i', 't'),
    HB_TAG ('p', 'r', 'e', 's'),
    HB_TAG ('a', 'b', 'v', 's'),
    HB_TAG ('b', 'l', 'w', 's'),
    HB_TAG ('p', 's', 't', 's'),
    HB_TAG ('h', 'a', 'l', 'n'),
    HB_TAG ('d', 'i', 's', 't'),
    HB_TAG ('a', 'b', 'v', 'm'),
    HB_TAG ('b', 'l', 'w', 'm'),
  };

  input->layout_features->add_array (default_layout_features, ARRAY_LENGTH (default_layout_features));
  return input;
}

/**
 * hb_subset_input_reference: (skip)
 * @input: a #hb_subset_input_t object.
 *
 * Return value:
 *
 * Since: 1.8.0
 **/
hb_subset_input_t *
hb_subset_input_reference (hb_subset_input_t *input)
{
  return hb_object_reference (input);
}

/**
 * hb_subset_input_destroy:
 * @input: a #hb_subset_input_t object.
 *
 * Since: 1.8.0
 **/
void
hb_subset_input_destroy (hb_subset_input_t *input)
{
  if (!hb_object_destroy (input)) return;

  hb_set_destroy (input->unicodes);
  hb_set_destroy (input->glyphs);
  hb_set_destroy (input->name_ids);
  hb_set_destroy (input->name_languages);
  hb_set_destroy (input->drop_tables);
  hb_set_destroy (input->layout_features);
  hb_set_destroy (input->no_subset_tables);

  hb_free (input);
}

/**
 * hb_subset_input_unicode_set:
 * @input: a #hb_subset_input_t object.
 *
 * Return value: pointer to the set of unicode codepoints to retain.
 *
 * Since: 1.8.0
 **/
HB_EXTERN hb_set_t *
hb_subset_input_unicode_set (hb_subset_input_t *input)
{
  return input->unicodes;
}

/**
 * hb_subset_input_glyph_set:
 * @input: a #hb_subset_input_t object.
 *
 * Return value: pointer to the set of glyph ids to retain.
 *
 * Since: 1.8.0
 **/
HB_EXTERN hb_set_t *
hb_subset_input_glyph_set (hb_subset_input_t *input)
{
  return input->glyphs;
}

/**
 * hb_subset_input_nameid_set:
 * @input: a #hb_subset_input_t object.
 *
 * Return value: pointer to the set of name ids to retain.
 *
 * Since: REPLACE
 **/
HB_EXTERN hb_set_t *
hb_subset_input_nameid_set (hb_subset_input_t *input)
{
  return input->name_ids;
}

/**
 * hb_subset_input_namelangid_set:
 * @input: a #hb_subset_input_t object.
 *
 * Return value: pointer to the set of name language ids to retain.
 *
 * Since: REPLACE
 **/
HB_EXTERN hb_set_t *
hb_subset_input_namelangid_set (hb_subset_input_t *input)
{
  return input->name_languages;
}


/**
 * hb_subset_input_layout_features_set:
 * @input: a #hb_subset_input_t object.
 *
 * Return value: pointer to the set of feature tags to retain.
 *
 * Since: REPLACE
 **/
HB_EXTERN hb_set_t *
hb_subset_input_layout_features_set (hb_subset_input_t *input)
{
  return input->layout_features;
}

HB_EXTERN void
hb_subset_input_set_retain_all_features (hb_subset_input_t *input,
					 hb_bool_t value)
{
  input->retain_all_layout_features = value;
}

HB_EXTERN hb_bool_t
hb_subset_input_get_retain_all_features (hb_subset_input_t *input)
{
  return input->retain_all_layout_features;
}


/**
 * hb_subset_input_drop_tabes_set:
 * @input: a #hb_subset_input_t object.
 *
 * Return value: pointer to the set of table tags which specifies tables
 *		 to be dropped.
 *
 * Since: REPLACE
 **/
HB_EXTERN hb_set_t *
hb_subset_input_drop_tables_set (hb_subset_input_t *input)
{
  return input->drop_tables;
}

/**
 * hb_subset_input_no_subset_tabes_set:
 * @input: a #hb_subset_input_t object.
 *
 * Return value: pointer to the set of table tags which specifies tables
 *		 that should not have subsetting applied to them.
 *
 * Since: REPLACE
 **/
HB_EXTERN hb_set_t *
hb_subset_input_no_subset_tables_set (hb_subset_input_t *input)
{
  return input->no_subset_tables;
}


/**
 * hb_subset_input_get_flag:
 * @input: a #hb_subset_input_t object.
 * @flag: which flag to check.
 *
 * Return value: value of the specified flag.
 *
 * Since: REPLACE
 **/
HB_EXTERN hb_bool_t
hb_subset_input_get_flag (hb_subset_input_t *input,
			  hb_subset_flag_t flag)
{
  switch (flag)
  {
    case HB_SUBSET_FLAG_HINTING:
      return !input->drop_hints;
    case HB_SUBSET_FLAG_RETAIN_GIDS:
      return input->retain_gids;
    case HB_SUBSET_FLAG_DESUBROUTINIZE:
      return input->desubroutinize;
    case HB_SUBSET_FLAG_NAME_LEGACY:
      return input->name_legacy;
    case HB_SUBSET_FLAG_SET_OVERLAPS_FLAG:
      return input->overlaps_flag;
    case HB_SUBSET_FLAG_PASSTHROUGH_UNRECOGNIZED:
      return input->passthrough_unrecognized;
    case HB_SUBSET_FLAG_NOTDEF_OUTLINE:
      return input->notdef_outline;
    case HB_SUBSET_FLAG_NO_PRUNE_UNICODE_RANGES:
      return input->no_prune_unicode_ranges;
    default:
      return false;
  }
}

/**
 * hb_subset_input_set_flag:
 * @input: a #hb_subset_input_t object.
 * @flag: which flag to set.
 * @value: new value for the flag.
 *
 * Set the specified flag to @value.
 *
 * Since: REPLACE
 **/
HB_EXTERN void
hb_subset_input_set_flag (hb_subset_input_t *input,
			  hb_subset_flag_t flag,
			  hb_bool_t value)
{
  switch (flag)
  {
    case HB_SUBSET_FLAG_HINTING:
      input->drop_hints = !value;
      break;
    case HB_SUBSET_FLAG_RETAIN_GIDS:
      input->retain_gids = value;
      break;
    case HB_SUBSET_FLAG_DESUBROUTINIZE:
      input->desubroutinize = value;
      break;
    case HB_SUBSET_FLAG_NAME_LEGACY:
      input->name_legacy = value;
      break;
    case HB_SUBSET_FLAG_SET_OVERLAPS_FLAG:
      input->overlaps_flag = value;
      break;
    case HB_SUBSET_FLAG_PASSTHROUGH_UNRECOGNIZED:
      input->passthrough_unrecognized = value;
      break;
    case HB_SUBSET_FLAG_NOTDEF_OUTLINE:
      input->notdef_outline = value;
      break;
    case HB_SUBSET_FLAG_NO_PRUNE_UNICODE_RANGES:
      input->no_prune_unicode_ranges = value;
      break;
    default:
      // Do nothing.
      break;
  }
}

/**
 * hb_subset_input_set_user_data: (skip)
 * @input: a #hb_subset_input_t object.
 * @key: The user-data key to set
 * @data: A pointer to the user data
 * @destroy: (nullable): A callback to call when @data is not needed anymore
 * @replace: Whether to replace an existing data with the same key
 *
 * Attaches a user-data key/data pair to the given subset input object.
 *
 * Return value: %true if success, %false otherwise
 *
 * Since: REPLACE
 **/
hb_bool_t
hb_subset_input_set_user_data (hb_subset_input_t  *input,
			       hb_user_data_key_t *key,
			       void *		   data,
			       hb_destroy_func_t   destroy,
			       hb_bool_t	   replace)
{
  return hb_object_set_user_data (input, key, data, destroy, replace);
}

/**
 * hb_subset_input_get_user_data: (skip)
 * @input: a #hb_subset_input_t object.
 * @key: The user-data key to query
 *
 * Fetches the user data associated with the specified key,
 * attached to the specified subset input object.
 *
 * Return value: (transfer none): A pointer to the user data
 *
 * Since: REPLACE
 **/
void *
hb_subset_input_get_user_data (const hb_subset_input_t *input,
			       hb_user_data_key_t     *key)
{
  return hb_object_get_user_data (input, key);
}
