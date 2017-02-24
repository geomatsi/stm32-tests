#ifndef DBC_ASSERT_H
#define DBC_ASSERT_H

/*
 * Simple implementation of Design-by-Contract approach using asserts
 * source: "Design by Contract (DbC) for Embedded Software" by Miro Samek
 * link: https://barrgroup.com/Embedded-Systems/How-To/Design-by-Contract-for-Embedded-Software
 */

/* NASSERT: disables all contract validations */

#ifdef NASSERT  /* NASSERT defined: DbC disabled */

#define DEFINE_THIS_FILE
#define ASSERT(expr)		((void)0)
#define ALLEGE(expr)		((void)(expr))

#else	/* NASSERT not defined: DbC enabled */

#ifdef __cplusplus
extern "C"
{
#endif

/* callback invoked in the case of assertion failure */
void on_assert__(char const *file, unsigned line);

#ifdef __cplusplus
}
#endif

#define DEFINE_THIS_FILE \
	static char const THIS_FILE__[] = __FILE__

#define ASSERT(expr) \
	((expr) ? (void)0 : on_assert__(THIS_FILE__, __LINE__))

#define ALLEGE(expr)	ASSERT(expr)

#endif	/* NASSERT */

#define REQUIRE(expr)   ASSERT(expr)
#define ENSURE(expr)    ASSERT(expr)
#define INVARIANT(expr) ASSERT(expr)

#endif	/* DBC_ASSERT_H */
