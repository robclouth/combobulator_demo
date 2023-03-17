import { observer } from "mobx-react";
import { JuceIntegration } from "./juceIntegration/JuceIntegration";

import {
  Image,
  Flex,
  MantineProvider,
  Title,
  useMantineTheme,
} from "@mantine/core";
import { MainPanel } from "./components/MainPanel";
import { ModelBrowser } from "./components/ModelBrowser";
import { Modulators } from "./components/Modulators";
import { ArcherContainer } from "react-archer";
import datamind from "./assets/images/datamind.png";

const App = observer(() => {
  const theme = useMantineTheme();

  return (
    <JuceIntegration>
      <MantineProvider withGlobalStyles withNormalizeCSS>
        <ArcherContainer
          strokeColor="black"
          strokeWidth={1}
          svgContainerStyle={{ zIndex: 99999, pointerEvents: "none" }}
        >
          <Flex direction="column" h="100vh">
            <Flex style={{ flex: 1 }} align="stretch">
              <MantineProvider theme={{ colorScheme: "dark" }}>
                <Flex w={300} miw={300} align="stretch">
                  <ModelBrowser />
                </Flex>
              </MantineProvider>
              <Flex direction="column" style={{ flex: 1 }}>
                <Title
                  order={1}
                  ff="Aldrich"
                  ta="center"
                  style={{
                    textTransform: "uppercase",
                    textShadow: "0px 10px 15px rgba(0, 0, 0, .25)",
                  }}
                  mt="lg"
                >
                  Combobulator
                </Title>
                <Flex style={{ flex: 1 }} align="center" justify="center">
                  <MainPanel />
                </Flex>
                <Image src={datamind} maw={150} mx="auto" mb="lg"></Image>
              </Flex>

              <MantineProvider theme={{ colorScheme: "dark" }}>
                <Flex w={300} miw={300} align="stretch">
                  <Modulators />
                </Flex>
              </MantineProvider>
            </Flex>
          </Flex>
        </ArcherContainer>
      </MantineProvider>
    </JuceIntegration>
  );
});

export default App;
