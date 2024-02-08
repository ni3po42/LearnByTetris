import { EventStream } from './eventStream.js';

describe('EventStream', () => {

    let eventStream;

    beforeEach(()=>{

        eventStream = new EventStream();

    });

    it('start', async () => {
    
        const stream = eventStream.start();

        const eventPromise = stream.next();

        eventStream.emit(1234);

        const result = await eventPromise;

        expect(result.value).toBe(1234);
        expect(result.done).toBeFalsy();
    });

    it('stop', async() => {
        const stream = eventStream.start();

        const eventPromise = stream.next();

        eventStream.stop();

        const result = await eventPromise;

        expect(result.value).toBeUndefined();
        expect(result.done).toBeTruthy();
    });

});
