/*
 * Copyright 2004 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING. If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */


/* see page 59, Annex C, table C.1 in the standard
 * offset word C' has been put at the end */
static const unsigned int offset_pos[5]={0,1,2,3,2};
static const unsigned int offset_word[5]={252,408,360,436,848};
static const unsigned int syndrome[5]={383,14,303,663,748};
static const char * const offset_name[]={"A","B","C","D","C'"};

/* optional message content, data field lengths and labels
 * see page 15 in ISO 14819-1 */
const int optional_content_lengths[16]={3,3,5,5,5,8,8,8,8,11,16,16,16,16,0,0};